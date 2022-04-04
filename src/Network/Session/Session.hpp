#ifndef SESSION_H
#define SESSION_H

#include "Coroutine/CoroutineTask.hpp"
#include "Misc/BaseFactoryObject.hpp"
#include "Misc/Timeouts.hpp"
#include "SessionForward.hpp"

class Socket;
class SessionReader;
class SessionWriter;

enum class SessionTimeouts {
    START,
    CLOSE
};

class Session : public CoroutineTask<void>, public BaseFactoryObject,
        public Timeouts<SessionTimeouts> {
public:
    Session(Socket*);
    ~Session();

    SessionReader& reader() const;
    SessionWriter& writer() const;

    template<typename Derived = Socket>
    Derived* socket() const {
        return static_cast<Derived*>(m_socket.get());
    }

    template<class TargetSession, typename... Args>
    std::shared_ptr<TargetSession> mutate(Args&&... args) {
        std::shared_ptr<TargetSession> result(new TargetSession(releaseSocket(), std::forward<Args>(args)...));
        return result;
    }

    // авто закрытие сокета
    void setAutoClose(bool autoClose);

protected:
    virtual TAwaitVoid prepare();
    virtual TAwaitVoid work();
    TAwaitVoid onStop() override;

    // закрыть сокет
    void close();

private:
    TAwaitVoid run() override final;

    void setSocket(Socket*);
    Socket* releaseSocket();

    bool m_autoClose;
    std::unique_ptr<Socket> m_socket;
    std::unique_ptr<SessionReader> m_reader;
    std::unique_ptr<SessionWriter> m_writer;
};

#endif // SESSION_H
