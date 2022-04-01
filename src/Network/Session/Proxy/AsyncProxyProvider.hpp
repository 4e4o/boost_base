#ifndef ASYNC_PROXY_PROVIDER_H
#define ASYNC_PROXY_PROVIDER_H

#include "AsyncProxyProviderDelegate.hpp"
#include "Network/Session/ISessionProvider.hpp"
#include "Misc/IntrusiveListHelpers.hpp"
#include "Coroutine/Awaitables.hpp"
#include "Coroutine/Spawn.hpp"

#include <unordered_map>

class AwaitableEvent;

class AsyncProxyProvider : public CoroutineSpawn, public ISessionProvider {
public:
    using TSessionClass = AsyncProxyProviderDelegate::TSessionClass;

    AsyncProxyProvider(boost::asio::io_context&, AsyncProxyProviderDelegate*);
    ~AsyncProxyProvider();

    void asyncResponse(TSession);

private:
    TAwaitSession get(ISessionRequester*) override final;

    struct TRequest : public TAutoUnlinkBaseListHook {
        TSession first;
        TSession second;
        std::shared_ptr<AwaitableEvent> event;
    };

    typedef TIntrusiveListAutoUnlink<TRequest> TRequests;

    AsyncProxyProviderDelegate* m_delegate;
    std::unordered_map<TSessionClass, TRequests> m_requests;
};

#endif // ASYNC_PROXY_PROVIDER_H
