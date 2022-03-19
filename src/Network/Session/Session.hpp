#ifndef SESSION_H
#define SESSION_H

#include "Misc/StrandHolder.hpp"
#include "Misc/TimeDuration.hpp"
#include "SessionForward.hpp"

#include <array>
#include <boost/signals2/signal.hpp>

#include <boost/date_time/posix_time/ptime.hpp>

class Timer;
class Socket;
class Operation;
class OperationsDispatcher;
class SessionReader;
class SessionWriter;

/**
 * @brief The Session class
 *
 * Гарантируется что сессия, если был вызван start(), всегда будет закрыта перед уничтожением.
 *
 * ! Всегда запускай сессию после создания. Если не запустил после создания, то нахуй создавал сессию?
 *   Если сессия умрёт не запущенной то сработает ассерт. Это нужно чтоб гарантировать что сессия
 *   всегда будет закрыта до деструктора.
 *
 */

class Session : public StrandHolder  {
public:
    enum class State {
        Initial,
        Starting,
        Started,
        Closing,
        Closed
    };

    enum class Timeouts {
        START,
        CLOSE,
        WRITE,
        READ
    };

    Session(Socket*);
    Session(Session*);
    virtual ~Session();

    void start();
    void close();
    void closeWaitWrite();

    template<typename Derived = Socket>
    Derived* socket() const {
        return static_cast<Derived*>(m_socket.get());
    }

    State state() const;

    SessionReader& reader() const;
    SessionWriter& writer() const;

    // юзать только после старта
    // если заюзать перед стартом то смысла не имеет, так как ожидание будет отменено
    void wait(const TTimeDuration&);

    TTimeDuration getTimeout(Timeouts);
    void setTimeout(Timeouts, const TTimeDuration& = boost::none);

    boost::signals2::signal<void()> starting;
    boost::signals2::signal<void()> started;
    boost::signals2::signal<void()> closing;
    boost::signals2::signal<void()> closed;

protected:
    virtual void startImpl();

private:
    static constexpr int READ_BUFFER_SIZE = 64 * 2 * 1024;

    void doStart();
    void setSocket(Socket*);
    Socket* releaseSocket();
    void disconnectAllSlots();
    void doClose(bool waitWrite);
    void trackDispatcher(OperationsDispatcher*);
    void checkStartCloseOp();
    void checkDispatchersInactivity();
    void initAllowInactivityTimer();
    void startAllowInactivity(const TTimeDuration&);
    void stopAllowInactivity();

    State m_state;
    TSession m_activeSelf;
    bool m_closeOnStart;
    bool m_closeOpStarted;
    bool m_allowInactivity;
    std::unique_ptr<Socket> m_socket;
    std::unique_ptr<Operation> m_startOp;
    std::unique_ptr<Operation> m_closeOp;
    std::unique_ptr<OperationsDispatcher> m_readOps;
    std::unique_ptr<OperationsDispatcher> m_writeOps;
    std::unique_ptr<OperationsDispatcher> m_stateOps;
    std::unique_ptr<SessionReader> m_reader;
    std::unique_ptr<SessionWriter> m_writer;
    std::shared_ptr<Timer> m_allowInactivityTimer;
    TTimeDuration m_initialInactivityDuration;
    std::unordered_map<Timeouts, TTimeDuration> m_timeouts;

    boost::optional<boost::posix_time::ptime> m_lastWaiTime;
    boost::optional<TTimeDuration> m_lastDUr;
};

#endif // SESSION_H
