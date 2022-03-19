#include "Server.hpp"
#include "Network/Session/Session.hpp"
#include "Network/Socket/TCP/SSLSocket.hpp"
#include "Misc/Lifecycle.hpp"

using namespace boost::asio;
using boost::system::error_code;

Server::Server(io_context &io)
    : StrandHolder(io),
      m_acceptor(io),
      m_socket(io),
      m_stopped(false) {
    registerDefaultType<Session, Socket*>();
    registerDefaultType<TCPSocket, io_context&, ip::tcp::socket&&>();
}

void Server::start(const std::string& ip, unsigned short port) {
    post<true>([this, ip, port] {
        if (m_stopped)
            return;

        m_acceptor = Acceptor(io(), ip::tcp::endpoint(ip::make_address_v4(ip), port));
        doAccept();
    });
}

void Server::enableSSL() {
    registerType<TCPSocket, SSLSocket, io_context&, ip::tcp::socket&&>();
}

void Server::stop() {
    post<true>([this] {
        if (m_stopped)
            return;

        m_stopped = true;
        error_code ec;
        m_acceptor.cancel(ec);
        stopped();
    });
}

void Server::doAccept() {
    m_acceptor.async_accept(m_socket,
                            bindExecutor([this](const error_code& ec) {
        if (m_stopped)
            return;

        if (!ec) {
            TCPSocket *socket = create<TCPSocket, io_context&, ip::tcp::socket&&>(io(), std::move(m_socket));
            TSession session(create<Session, Socket*>(socket));
            newSession(session);
            Lifecycle::connectTrack(stopped, session, &Session::close);
            session->start();
        }

        doAccept();
    }));
}
