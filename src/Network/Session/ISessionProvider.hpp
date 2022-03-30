#ifndef I_SESSION_PROVIDER_H
#define I_SESSION_PROVIDER_H

#include "Network/Session/SessionForward.hpp"

#include <boost/asio/awaitable.hpp>

typedef boost::asio::awaitable<TSession> TAwaitSession;

class ISessionRequester;

class ISessionProvider {
public:
    ISessionProvider() { }
    virtual ~ISessionProvider() { }

    virtual TAwaitSession get(ISessionRequester*) = 0;
};

#endif // I_SESSION_PROVIDER_H
