#ifndef COROUTINE_TASK_HPP
#define COROUTINE_TASK_HPP

#include "Spawn.hpp"
#include "Awaitables.hpp"
#include "CoroutineTimerHelpers.hpp"

#include "Misc/TimeDuration.hpp"
#include "Misc/Lifecycle.hpp"
#include "Misc/ScopeGuard.hpp"

#include <boost/signals2/signal.hpp>

template<typename TResult, typename... Args>
class CoroutineTask : public CoroutineSpawn, public CoroutineTimerHelpers {
public:
    typedef boost::asio::awaitable<TResult> TAwaitResult;

    CoroutineTask(boost::asio::io_context &io)
        : CoroutineSpawn(io), CoroutineTimerHelpers(this), m_state(State::INITIAL) { }
    ~CoroutineTask() { }

    void start(Args&&... args) {
        co_start(boost::asio::detached, std::forward<Args>(args)...);
    }

    template<typename CompletionToken = decltype(boost::asio::detached)>
    auto co_start(CompletionToken ct = boost::asio::detached, Args... args) {
        return spawn<true>([this, args...](TSpawnCancellation cancel) -> TAwaitResult {
            co_return co_await work(cancel, args...);
        }, ct);
    }

    void stop() {
        co_stop();
    }

    template<typename CompletionToken = decltype(boost::asio::detached)>
    auto co_stop(CompletionToken ct = boost::asio::detached) {
        return spawn<true>([this]() -> TAwaitVoid {
            if (m_state == State::STOPPED)
                co_return;

            m_state = State::STOPPED;
            co_await onStop();

            if (m_cancellation) {
                m_cancellation();
            }

            m_stopped();
            co_return;
        }, ct);
    }

protected:
    virtual TAwaitResult run(Args...) = 0;
    virtual TAwaitVoid onStop() {
        co_return;
    }

    bool running() const {
        return m_state == State::RUNNING;
    }

    static void throwGenericCoroutineError() {
        using namespace boost::system;
        throw system_error(errc::make_error_code(errc::state_not_recoverable));
    }

    template<typename Task>
    void registerStop(const std::shared_ptr<Task>& t) {
        Lifecycle::connectTrack(m_stopped, t, &Task::stop);
    }

private:
    TAwaitResult work(TSpawnCancellation cancel, Args... args) {
        initTimer();

        using namespace boost::asio;
        using namespace boost::system;

        if (m_state != State::INITIAL) {
            throwGenericCoroutineError();
        }

        m_cancellation = cancel;
        co_return co_await work0(args...);
    }

    TAwaitResult work0(Args... args) {
        ScopeGuard sg([this] {
            if (m_state != State::STOPPED) {
                m_state = State::DONE;
            }
        });

        m_state = State::RUNNING;
        co_return co_await run(args...);
    }

    enum class State {
        INITIAL,
        RUNNING,
        DONE,
        STOPPED
    };

    State m_state;
    TSpawnCancellation m_cancellation;
    boost::signals2::signal<void()> m_stopped;
};

#endif /* COROUTINE_TASK_HPP */
