#ifndef PROXY_H
#define PROXY_H

#include "Network/Session/SessionForward.hpp"
#include "Coroutine/CoroutineTask.hpp"
#include "IProxySessionRequester.hpp"
#include "Misc/Timeouts.hpp"

class ISessionProvider;

enum class ProxyTimeouts {
    GET_SESSION,
    FIRST_WORK,
    SECOND_WORK
};

class Proxy : public CoroutineTask<void>, public IProxySessionRequester,
        public Timeouts<ProxyTimeouts> {
public:
    typedef std::shared_ptr<Proxy> TProxy;

    Proxy(boost::asio::io_context&, TSession s, ISessionProvider*);
    ~Proxy();

protected:
    TAwaitVoid run() override;

private:
    TSession getInitiationSession() const override;

    TSession m_first;
    ISessionProvider* m_provider;
};

#endif // PROXY_H
