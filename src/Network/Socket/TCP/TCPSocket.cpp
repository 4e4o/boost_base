#include "TCPSocket.hpp"
#include "Misc/Debug.hpp"

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

#define TCP_KEEPIDLE_VALUE      20
#define TCP_KEEPINTVL_VALUE     10
#define TCP_KEEPCNT_VALUE       4

using boost::asio::ip::tcp;
using boost::asio::socket_base;
using boost::asio::detail::socket_option::integer;

TCPSocket::TCPSocket(boost::asio::io_context &io)
    : TCPSocket(io, boost::asio::ip::tcp::socket(io)) {
}

TCPSocket::TCPSocket(boost::asio::io_context &io, boost::asio::ip::tcp::socket&& socket)
    : Socket(io),
      m_socket(new TSocket(std::move(socket))),
      m_socketOwner(true) {
}

TCPSocket::~TCPSocket() {    
    if (!m_socketOwner) {
        m_socket.release();
    }
}

void TCPSocket::setTCPSocket(boost::asio::ip::tcp::socket *s) {
    if (!m_socketOwner) {
        m_socket.release();
    }

    m_socket.reset(s);
    m_socketOwner = false;
}

void TCPSocket::setNagle(bool enable) {
    m_socket->set_option(tcp::no_delay(!enable));
}

void TCPSocket::setKeepAlive(bool enable) {
    m_socket->set_option(socket_base::keep_alive(enable));

    if (enable) {
        m_socket->set_option(integer<SOL_TCP, TCP_KEEPIDLE>(TCP_KEEPIDLE_VALUE));       // secs before keepalive probes
        m_socket->set_option(integer<SOL_TCP, TCP_KEEPINTVL>(TCP_KEEPINTVL_VALUE));     // interval between keepalive
        m_socket->set_option(integer<SOL_TCP, TCP_KEEPCNT>(TCP_KEEPCNT_VALUE));         // failed keepalive before declaring dead
    }
}

void TCPSocket::asyncStartImpl(const TErrorCallback& c) {
    post([this, c] {
        setNagle(false);
        setKeepAlive(true);
        boost::system::error_code ec;
        c(ec);
    });
}

void TCPSocket::async_close(const TErrorCallback& c) {
    post([this, c] {
        force_close(c);
    });
}

void TCPSocket::async_read_some(uint8_t* ptr, const std::size_t &size, const TDataCallback &c) {
    m_socket->async_read_some(boost::asio::buffer(ptr, size), bindExecutor(c));
}

void TCPSocket::async_read_all(uint8_t* ptr, const std::size_t &size, const TDataCallback &c) {
    boost::asio::async_read(*m_socket, boost::asio::buffer(ptr, size), bindExecutor(c));
}

void TCPSocket::async_write_all(const uint8_t*ptr, const std::size_t& size, const TDataCallback& c) {
    boost::asio::async_write(*m_socket, boost::asio::buffer(ptr, size), bindExecutor(c));
}

void TCPSocket::cancel() {
    debug_print(boost::format("TCPSocket::cancel %1%") % this);
    boost::system::error_code ec;
    m_socket->cancel(ec);
}

void TCPSocket::force_close(const TErrorCallback& c) {
    debug_print(boost::format("TCPSocket::force_close %1%") % this);
    boost::system::error_code ec1, ec2;
    m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec1);
    m_socket->close(ec2);
    c(ec2);
}

Socket* TCPSocket::create(boost::asio::io_context &io) {
    return new TCPSocket(io);
}
