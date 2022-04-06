#include "AsyncProxyProvider.hpp"
#include "IProxySessionRequester.hpp"
#include "Coroutine/AwaitableEvent.hpp"
#include "Misc/Debug.hpp"

using namespace boost::asio;
using namespace boost::system;

AsyncProxyProvider::AsyncProxyProvider(boost::asio::io_context& io, AsyncProxyProviderDelegate* d)
    : CoroutineSpawn(io), m_delegate(d) {
}

AsyncProxyProvider::~AsyncProxyProvider() {
}

void AsyncProxyProvider::asyncResponse(TSession second) {
    post<true>([this, second]() {
        TSessionClass sClass = m_delegate->classify(second);
        auto rit = m_requests.find(sClass);

        if (rit == m_requests.end() || rit->second.empty()) {
            debug_print_this(fmt("no requests with sClass = %1%, %2%") % sClass % second.get());
            return;
        }

        TRequests& requests = rit->second;
        TRequest& request = requests.front();
        requests.pop_front();

        debug_print_this(fmt("%1% %2%") % sClass % requests.size());

        request.second = second;
        request.event->set();
    });
}

TAwaitSession AsyncProxyProvider::get(ISessionRequester* r) {
    return spawn<true>([this, r]() -> TAwaitSession {
        STRAND_ASSERT(this);
        IProxySessionRequester* requster = static_cast<IProxySessionRequester*>(r);
        TSession first = requster->getInitiationSession();
        TSessionClass rClass = m_delegate->classify(first);

        if (m_requests.find(rClass) == m_requests.end())
            m_requests[rClass] = TRequests();

        TRequests& requests = m_requests[rClass];
        std::unique_ptr<TRequest> request(new TRequest());

        request->event.reset(new AwaitableEvent(executor()));
        request->first = first;

        requests.push_back(*request);

        debug_print_this(fmt("%1% %2%") % rClass % requests.size());

        if (!co_await m_delegate->startAsyncRequest(rClass)) {
            STRAND_ASSERT(this);
            throw errc::make_error_code(errc::host_unreachable);
        }

        STRAND_ASSERT(this);
        co_await request->event->co_wait();
        STRAND_ASSERT(this);
        TSession result = request->second;

        if (result.get() == nullptr) {
            throw errc::make_error_code(errc::host_unreachable);
        }

        co_return result;
    }, use_awaitable);
}
