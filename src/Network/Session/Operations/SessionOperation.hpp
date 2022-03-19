#ifndef SESSION_OPERATION_H
#define SESSION_OPERATION_H

#include "Misc/TimeDuration.hpp"
#include "Operations/Operation.hpp"

class Session;
class Socket;
class Timer;

class SessionOperation : public Operation {
public:
    typedef std::shared_ptr<Session> TSession;

    SessionOperation(TSession, const TTimeDuration& = boost::none);
    ~SessionOperation();

protected:
    void startImpl() override;
    void finish(bool);

    Socket* socket() const;
    TSession session() const;

private:
    std::shared_ptr<Timer> m_timeout;
    std::weak_ptr<Session> m_weakSession;
};

#endif // SESSION_OPERATION_H
