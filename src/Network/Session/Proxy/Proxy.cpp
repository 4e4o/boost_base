#include "Proxy.hpp"
#include "Network/Session/ISessionProvider.hpp"
#include "Network/Session/Session.hpp"
#include "ProxyDataSession.hpp"
#include "Misc/Debug.hpp"

using namespace std::literals::chrono_literals;

//#define DEFAULT_TIMEOUT  1s
#define DEFAULT_TIMEOUT  10s

using namespace boost::asio;

Proxy::Proxy(io_context&io, TSession s, ISessionProvider *p)
    : CoroutineTask(io),
      m_first(s),
      m_provider(p) {
    debug_print_this("");

    setTimeout(Timeout::GET_SESSION, DEFAULT_TIMEOUT);
    setTimeout(Timeout::FIRST_WORK, DEFAULT_TIMEOUT);
    setTimeout(Timeout::SECOND_WORK, DEFAULT_TIMEOUT);
}

Proxy::~Proxy() {
    debug_print_this("");
}

TSession Proxy::getInitiationSession() const {
    return m_first;
}

TAwaitVoid Proxy::run() {
    TSession second = co_await timeout(m_provider->get(this), getTimeout(Timeout::GET_SESSION));
    co_await timeout(m_first->co_start(use_awaitable), getTimeout(Timeout::FIRST_WORK));
    co_await timeout(second->co_start(use_awaitable), getTimeout(Timeout::SECOND_WORK));
    TProxyDataSession p1 = m_first->mutate<ProxyDataSession>();
    TProxyDataSession p2 = second->mutate<ProxyDataSession>();
    m_first.reset();
    second.reset();
    p1->setOther(p2);
    p2->setOther(p1);
    p1->start();
    co_return co_await p2->co_start(use_awaitable);
}
