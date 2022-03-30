#ifndef COROUTINE_TIMER_HELPERS_HPP
#define COROUTINE_TIMER_HELPERS_HPP

#include "Awaitables.hpp"
#include "Concepts.hpp"
#include "Timed/Timed.hpp"
#include "Misc/StrandHolder.hpp"

#include <boost/asio/use_awaitable.hpp>

class CoroutineTimerHelpers {
protected:
    CoroutineTimerHelpers(StrandHolder* s)
        : m_strand(s) { }

    void initTimer() {
        m_timer.reset(new boost::asio::steady_timer(m_strand->executor()));
    }

    static constexpr auto deferred = boost::asio::experimental::deferred;
    static constexpr auto use_awaitable = boost::asio::use_awaitable;

    template <DeferredAsyncOp Op>
    auto timeout(Op&& op, const TDurationUnit& d) {
        return timeout(std::forward<Op>(op), TTimeDuration(d));
    }

    template <DeferredAsyncOp Op>
    auto timeout(Op&& op, const TTimeDuration& timeout = boost::none) {
        using namespace boost::asio;

        if (timeout.has_value()) {
            return with_timeout(std::forward<Op>(op), m_timer.get(), *timeout, use_awaitable);
        }

        return std::forward<Op>(op)(use_awaitable);
    }

    template <Awaitable Op>
    auto timeout(Op&& op, const TDurationUnit& d) {
        return timeout(std::forward<Op>(op), TTimeDuration(d));
    }

    template <Awaitable Op>
    auto timeout(Op&& op, const TTimeDuration& timeout = boost::none) {
        using namespace boost::asio;

        if (timeout.has_value()) {
            auto deferredOp = m_strand->spawn([op = std::move(op)]() mutable -> Op {
                // FIXME return
                //co_return co_await std::move(op);

                if constexpr (std::is_same_v<Op, TAwaitVoid>) {
                    co_await std::move(op);
                    co_return;

                } else {
                    auto res = co_await std::move(op);
                    co_return res;
                }

            }, deferred);

            return with_timeout(std::move(deferredOp), m_timer.get(), *timeout, use_awaitable);
        }

        return std::move(op);
    }

    TAwaitVoid wait(const TDurationUnit& d) {
        m_timer->expires_after(d);
        co_await m_timer->async_wait(use_awaitable);
    }

private:
    StrandHolder *m_strand;
    std::unique_ptr<boost::asio::steady_timer> m_timer;
};

#endif /* COROUTINE_TIMER_HELPERS_HPP */
