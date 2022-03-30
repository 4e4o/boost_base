#include "Server.hpp"
#include "Network/Session/Session.hpp"
#include "Network/Socket/TCP/SSLSocket.hpp"
#include "Misc/Debug.hpp"

using boost::system::error_code;
using namespace boost::asio;

Server::Server(io_context &io)
    : CoroutineTask(io) {
    debug_print_this("");

    registerDefaultType<Session, Socket*>();
    registerDefaultType<TCPSocket, io_context&, ip::tcp::socket&&>();
}

Server::~Server() {
    debug_print_this("");
}

void Server::enableSSL() {
    registerType<TCPSocket, SSLSocket, io_context&, ip::tcp::socket&&>();
}

void Server::setHandler(const TNewSessionHandler &handler) {
    m_handler = handler;
}

Server::TAwaitResult Server::run(const std::string& ip, unsigned short port) {
    STRAND_ASSERT(this);

    using Acceptor = boost::asio::ip::tcp::acceptor;
    auto acceptor = Acceptor(io(), ip::tcp::endpoint(ip::make_address_v4(ip), port));

    while(running()) {
        STRAND_ASSERT(this);
        auto sock = co_await acceptor.async_accept(use_awaitable);
        STRAND_ASSERT(this);
        TCPSocket *socket = create<TCPSocket, io_context&, ip::tcp::socket&&>(io(), std::move(sock));
        TSession session(create<Session, Socket*>(socket));
        registerStop(session);

        if (m_handler) {
            if (m_handler(session)) {
                session->start();
            }
        } else {
            session->start();
        }
    }
}
