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
    TAwaitVoid onStop() override;

private:
    enum class State {
        READ,
        WRITE
    };

    void otherDone(State);

    TWProxyDataSession m_other;
    State m_state;
    bool m_otherDone;
};

#endif // PROXY_DATA_SESSION_H
