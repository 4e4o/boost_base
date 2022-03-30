#ifndef PROXY_DATA_SESSION_H
#define PROXY_DATA_SESSION_H

#include "Network/Session/Session.hpp"
#include "ProxyDataSessionForward.hpp"

// просто прокси сессия, которая передаёт данные другой прокси сессии и получает из неё же.

class ProxyDataSession : public Session {
public:
    ProxyDataSession(Socket*);
    ~ProxyDataSession();

    void setOther(TProxyDataSession);

protected:
    TAwaitVoid work() override;

private:
    void otherDone();

    TWProxyDataSession m_other;
    bool m_activityDetect;
    bool m_activity;
};

#endif // PROXY_DATA_SESSION_H
