#ifndef SERVER_H
#define SERVER_H

#include "ServerForward.hpp"
#include "Misc/GenericFactory.hpp"
#include "Network/Session/SessionForward.hpp"
#include "Coroutine/CoroutineTask.hpp"

class Server : public CoroutineTask<void, const std::string&, unsigned short>,
        public GenericFactory<> {
public:
    Server(boost::asio::io_context &io);
    ~Server();

    void enableSSL();

    typedef std::function<bool(TWSession)> TNewSessionHandler;
    void setHandler(const TNewSessionHandler &handler);

private:
    TAwaitResult run(const std::string& ip, unsigned short port) override;
    TNewSessionHandler m_handler;
};

#endif // SERVER_H
