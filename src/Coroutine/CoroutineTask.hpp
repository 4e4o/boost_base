#ifndef COROUTINE_TASK_HPP
#define COROUTINE_TASK_HPP

#include "Misc/TimeDuration.hpp"
#include "Misc/StrandHolder.hpp"
#include "Misc/Lifecycle.hpp"
#include "Misc/ScopeGuard.hpp"
#include "Coroutine/Awaitables.hpp"
#include "Coroutine/CoroutineTimerHelpers.hpp"

#include <boost/signals2/signal.hpp>

#include <boost/asio/bind_cancellation_slot.hpp>

template<typename TResult, typename... Args>
class CoroutineTask : public StrandHolder, public CoroutineTimerHelpers {
public:
    typedef boost::asio::awaitable<TResult> TAwaitResult;

    CoroutineTask(boost::asio::io_context &io)
        : StrandHolder(io), CoroutineTimerHelpers(this), m_state(State::INITIAL) { }
    ~CoroutineTask() { }

    void start(Args&&... args) {
        co_start(boost::asio::detached, std::forward<Args>(args)...);
    }

    template<typename CompletionToken = decltype(boost::asio::detached)>
    auto co_start(CompletionToken ct = boost::asio::detached, Args... args) {
        return spawn<true>([this, args...]() -> TAwaitResult {
            co_return co_await work(args...);
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
            emitCancellation();
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
    TAwaitResult work(Args... args) {
        initTimer();

        using namespace boost::asio;
        using namespace boost::system;

        if (m_state != State::INITIAL) {
            throwGenericCoroutineError();
        }

        // для co_spawn которые были вызваны с bind_cancellation_slot сигналы отмены не распространяются
        // автоматически во вложенные co_spawn, нужно руками связывать перед co_await
        cancellation_state cs = co_await this_coro::cancellation_state;
        cancellation_slot slot = cs.slot();

        // ставим слот если только родительский(текущий co_spawn) имеет слот отмены
        // не иметь слот отмены он может если например запущен как detached
        if (slot.is_connected()) {
            slot.assign([this](auto) {
                emitCancellation();
            });
        }

        // еще раз спавним поток корутины чтоб на этот раз наверняка поиметь m_cancellation функционал
        co_return co_await spawn([this, &args...]() -> TAwaitResult {
            co_return co_await work0(args...);
        }, bind_cancellation_slot(m_cancellation.slot(), use_awaitable));
    }

    void emitCancellation() {
        dispatch<true>([this] {
            m_cancellation.emit(boost::asio::cancellation_type::all);
        });
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
    boost::signals2::signal<void()> m_stopped;
    boost::asio::cancellation_signal m_cancellation;
};

#endif /* COROUTINE_TASK_HPP */
