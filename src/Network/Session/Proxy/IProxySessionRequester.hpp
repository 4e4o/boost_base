#ifndef I_PROXY_SESSION_REQUESTER_H
#define I_PROXY_SESSION_REQUESTER_H

#include "Network/Session/SessionForward.hpp"
#include "Network/Session/ISessionRequester.hpp"

class IProxySessionRequester : public ISessionRequester {
public:
    IProxySessionRequester() { }
    ~IProxySessionRequester() { }

    virtual TSession getInitiationSession() const = 0;
};

#endif // I_PROXY_SESSION_REQUESTER_H
