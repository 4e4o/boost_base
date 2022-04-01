#ifndef COROUTINE_TIMER_HELPERS_HPP
#define COROUTINE_TIMER_HELPERS_HPP

#include "Awaitables.hpp"
#include "Concepts.hpp"
#include "Timed/Timed.hpp"
#include "Spawn.hpp"

#include <boost/asio/use_awaitable.hpp>

class CoroutineTimerHelpers {
protected:
    CoroutineTimerHelpers(CoroutineSpawn* s)
        : m_spawn(s) { }

    void initTimer() {
        m_timer.reset(new boost::asio::steady_timer(m_spawn->executor()));
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
            auto deferredOp = m_spawn->spawn([op = std::move(op)]() mutable -> Op {
                co_return co_await std::move(op);
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
    CoroutineSpawn *m_spawn;
    std::unique_ptr<boost::asio::steady_timer> m_timer;
};

#endif /* COROUTINE_TIMER_HELPERS_HPP */
