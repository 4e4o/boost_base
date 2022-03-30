#ifndef PROXY_DATA_SESSION_FORWARD_HPP
#define PROXY_DATA_SESSION_FORWARD_HPP

#include <memory>

class ProxyDataSession;

typedef std::shared_ptr<ProxyDataSession> TProxyDataSession;
typedef std::weak_ptr<ProxyDataSession> TWProxyDataSession;

#endif // PROXY_DATA_SESSION_FORWARD_HPP
