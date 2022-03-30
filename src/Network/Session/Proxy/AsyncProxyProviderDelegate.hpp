#ifndef ASYNC_PROXY_PROVIDER_DELEGATE_HPP
#define ASYNC_PROXY_PROVIDER_DELEGATE_HPP

#include "Coroutine/Awaitables.hpp"
#include "Network/Session/SessionForward.hpp"

class AsyncProxyProviderDelegate {
public:
    AsyncProxyProviderDelegate() { }
    virtual ~AsyncProxyProviderDelegate() { }

    typedef int TSessionClass;

    virtual TSessionClass classify(TSession) = 0;
    virtual TAwaitBool startAsyncRequest(TSessionClass) = 0;
};

#endif // ASYNC_PROXY_PROVIDER_DELEGATE_HPP
