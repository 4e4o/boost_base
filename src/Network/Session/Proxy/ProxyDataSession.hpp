#ifndef PROXY_DATA_SESSION_H
#define PROXY_DATA_SESSION_H

#include "Network/Session/Session.hpp"

// просто прокси сессия, которая передаёт данные другой прокси сессии и получает из неё же.

class ProxyDataSession : public Session {
public:
    typedef std::shared_ptr<ProxyDataSession> TSessionPtr;

    static TSessionPtr proxy(std::shared_ptr<Session>, std::shared_ptr<Session>);

    ~ProxyDataSession();

private:
    ProxyDataSession(Session*);

    virtual void startImpl() override final;
    void setOther(const TSessionPtr&);

    TSessionPtr m_other;
};

#endif // PROXY_DATA_SESSION_H
