#ifndef STRAND_HOLDER_HPP
#define STRAND_HOLDER_HPP

#include "Misc/EnableSharedFromThisVirtual.hpp"
#include "Misc/CallableTraits.hpp"
#include "IOContextHolder.hpp"
#include "StrandExecutor.hpp"

#define STRAND_ASSERT(s) assert((s)->running_in_strand_thread())

class StrandHolder : public enable_shared_from_this_virtual<StrandHolder>, public IOContextHolder {
public:
    StrandHolder(boost::asio::io_context&);
    virtual ~StrandHolder();

    void setStrand(StrandHolder*);

    template <bool SelfLock = false, class Callable>
    void post(Callable&& c) const {
        boost::asio::post(executor(), autoLockHandler<SelfLock>(std::forward<Callable>(c)));
    }

    template <bool SelfLock = false, class Callable>
    void dispatch(Callable&& c) const {
        boost::asio::dispatch(executor(), autoLockHandler<SelfLock>(std::forward<Callable>(c)));
    }

    bool running_in_strand_thread() const;

    TStrandExecutor& executor() const;

protected:
    template<bool SelfLock = false, class Callable>
    auto autoLockHandler(Callable&& c) const {
        return autoLockHandlerImpl<SelfLock>(std::forward<Callable>(c));
    }

private:
    template <typename T>
    struct expandAutoLockTuple;

    template <typename... Args>
    struct expandAutoLockTuple<std::tuple<Args...>> {
        template<class Callable, typename T>
        static auto createWrapper(Callable &&c, T& self) {
            typedef typename CallableTraits<Callable>::result_type TResult;
            auto wrapper = [self, c = std::move(c)](Args... args) -> TResult {
                return c(std::forward<Args>(args)...);
            };
            return std::move(wrapper);
        }
    };

    template<bool SelfLock = false, class Callable>
    auto autoLockHandlerImpl(Callable&& c) const {
        if constexpr (SelfLock) {
            typedef typename CallableTraits<Callable>::args_tuple TArgsTuple;
            auto self = shared_from_this();
            return expandAutoLockTuple<TArgsTuple>::createWrapper(std::forward<Callable>(c), self);
        } else {
            return std::forward<Callable>(c);
        }
    }

    std::unique_ptr<TStrandExecutor> m_executor;
    StrandHolder *m_other;
};

#endif /* STRAND_HOLDER_HPP */
