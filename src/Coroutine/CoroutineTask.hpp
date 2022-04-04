#ifndef COROUTINE_TASK_HPP
#define COROUTINE_TASK_HPP

#include "Spawn.hpp"
#include "CoroutineTimerHelpers.hpp"

#include "Misc/Lifecycle.hpp"
#include "Misc/ScopeGuard.hpp"

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
            STRAND_ASSERT(this);

            if (m_state == State::STOPPED)
                co_return;

            m_state = State::STOPPED;
            co_await onStop();
            STRAND_ASSERT(this);

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
        STRAND_ASSERT(this);
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
        using namespace boost::asio;
        using namespace boost::system;

        initTimer();

        if (m_state != State::INITIAL) {
            throwGenericCoroutineError();
        }

        m_cancellation = cancel;
        ScopeGuard sg([this] {
            STRAND_ASSERT(this);

            if (m_state != State::STOPPED) {
                m_state = State::DONE;
            }
        });

        m_state = State::RUNNING;

        try {
            co_return co_await run(args...);
        } catch(const system_error& e) {
            // для operation_canceled останавливаем текущий таск
            // это для случая когда один CoroutineTask ко-эвэйтит на другом таске.
            // При остановке родительского таска стоп будет вызван явно, а вот во вложенные таски
            // прийдет только сигнал отмены, чтоб уровнять состояния всех вложенных тасков - останавливаем.
            if (e.code() == errc::operation_canceled) {
                stop();
            }

            throw;
        }
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
