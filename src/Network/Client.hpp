#ifndef CLIENT_H
#define CLIENT_H

#include "ClientForward.hpp"
#include "Misc/GenericFactory.hpp"
#include "Coroutine/CoroutineTask.hpp"
#include "Network/Session/SessionForward.hpp"

class Client : public CoroutineTask<TSession, const std::string&, unsigned short>,
        public GenericFactory<BaseFactoryObject> {
public:
    Client(boost::asio::io_context&, const TDurationUnit&);
    Client(boost::asio::io_context&, const TTimeDuration& = boost::none);
    ~Client();

    void enableSSL();
    bool connected() const;
    void setConnectTimeout(const TTimeDuration &);

    typedef std::function<void(TWSession)> TNewSessionHandler;

    void setHandler(const TNewSessionHandler &handler);

protected:
    TAwaitResult run(const std::string& ip, unsigned short port) override;

private:
    TWSession m_session;
    const bool m_managedMode;
    TTimeDuration m_connect;
    const TTimeDuration m_reconnect;
    TNewSessionHandler m_handler;
};

#endif // CLIENT_H
