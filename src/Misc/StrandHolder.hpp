#ifndef STRAND_HOLDER_HPP
#define STRAND_HOLDER_HPP

#include "Misc/EnableSharedFromThisVirtual.hpp"

#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/bind_executor.hpp>

#define STRAND_ASSERT(s) assert((s)->running_in_strand_thread())

class StrandHolder : public enable_shared_from_this_virtual<StrandHolder> {
public:
    StrandHolder(boost::asio::io_context&);
    virtual ~StrandHolder();

    boost::asio::io_context& io() const;

    void setStrand(StrandHolder*);

    template<class Callable>
    auto bindExecutor(Callable&& c) {
        return boost::asio::bind_executor(strand(), std::forward<Callable>(c));
    }

    template <bool Lock = false, class Callable>
    void post(Callable&& c) {
        if constexpr (Lock) {
            boost::asio::post(bindExecutor(wrapLocked(std::forward<Callable>(c))));
        } else {
            boost::asio::post(bindExecutor(std::forward<Callable>(c)));
        }
    }

    template <bool Lock = false, class Callable>
    void dispatch(Callable&& c) {
        if constexpr (Lock) {
            boost::asio::dispatch(bindExecutor(wrapLocked(std::forward<Callable>(c))));
        } else {
            boost::asio::dispatch(bindExecutor(std::forward<Callable>(c)));
        }
    }

    bool running_in_strand_thread() const;

private:
    boost::asio::io_context::strand& strand() const;

    template<class Callable>
    auto wrapLocked(Callable&& c) {
        auto self = shared_from_this();
        auto wrapper = [self, c = std::move(c)] {
            c();
        };
        return std::move(wrapper);
    }

    boost::asio::io_context& m_io;
    std::shared_ptr<boost::asio::io_context::strand> m_strand;
    StrandHolder *m_other;
};

#endif /* STRAND_HOLDER_HPP */
