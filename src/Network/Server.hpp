#ifndef SERVER_H
#define SERVER_H

#include <boost/signals2.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "Misc/StrandHolder.hpp"
#include "Misc/GenericFactory.hpp"
#include "Network/Session/SessionForward.hpp"
#include "ServerForward.hpp"

class Server : public StrandHolder, public GenericFactory<StrandHolder> {
public:
    using Acceptor = boost::asio::ip::tcp::acceptor;

    Server(boost::asio::io_context &io);

    void start(const std::string& ip, unsigned short port);
    void stop();

    void enableSSL();

    boost::signals2::signal<void(TWSession)> newSession;
    boost::signals2::signal<void()> stopped;

private:
    void doAccept();

    Acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_socket;
    bool m_stopped;
};

#endif // SERVER_H
