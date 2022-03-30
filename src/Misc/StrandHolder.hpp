#ifndef STRAND_HOLDER_HPP
#define STRAND_HOLDER_HPP

#include "Misc/EnableSharedFromThisVirtual.hpp"
#include "IOContextHolder.hpp"
#include "StrandExecutor.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

#define STRAND_ASSERT(s) assert((s)->running_in_strand_thread())

class StrandHolder : public enable_shared_from_this_virtual<StrandHolder>, public IOContextHolder {
public:
    StrandHolder(boost::asio::io_context&);
    virtual ~StrandHolder();

    void setStrand(StrandHolder*);

    template <bool SelfLock = false, typename CompletionToken = decltype(boost::asio::detached), class Callable>
    auto spawn(Callable&& c, CompletionToken ct = boost::asio::detached) const {
        return boost::asio::co_spawn(executor(), wrap<SelfLock>(std::forward<Callable>(c)), ct);
    }

    template <bool SelfLock = false, class Callable>
    void post(Callable&& c) const {
        boost::asio::post(executor(), wrap<SelfLock>(std::forward<Callable>(c)));
    }

    template <bool SelfLock = false, class Callable>
    void dispatch(Callable&& c) const {
        boost::asio::dispatch(executor(), wrap<SelfLock>(std::forward<Callable>(c)));
    }

    bool running_in_strand_thread() const;

    TStrandExecutor& executor() const;

private:
    template<bool SelfLock = false, class Callable>
    auto wrap(Callable&& c) const {
        if constexpr (SelfLock) {
            auto self = shared_from_this();
            auto wrapper = [self, c = std::move(c)]() -> auto {
                return c();
            };
            return std::move(wrapper);
        } else {
            return std::forward<Callable>(c);
        }
    }

    std::unique_ptr<TStrandExecutor> m_executor;
    StrandHolder *m_other;
};

#endif /* STRAND_HOLDER_HPP */
