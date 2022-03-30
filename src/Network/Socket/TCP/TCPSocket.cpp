#include "TCPSocket.hpp"
#include "Misc/Debug.hpp"

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/use_awaitable.hpp>

#define TCP_KEEPIDLE_VALUE      20
#define TCP_KEEPINTVL_VALUE     10
#define TCP_KEEPCNT_VALUE       4

using namespace boost::asio;
using boost::asio::ip::tcp;
using boost::asio::socket_base;
using boost::asio::detail::socket_option::integer;

TCPSocket::TCPSocket(io_context &io)
    : TCPSocket(io, ip::tcp::socket(io)) {
}

TCPSocket::TCPSocket(boost::asio::io_context& io, ip::tcp::socket&& socket)
    : Socket(io),
      m_socket(new TSocket(std::move(socket))),
      m_socketOwner(true) {
}

TCPSocket::~TCPSocket() {
    if (!m_socketOwner) {
        m_socket.release();
    }
}

void TCPSocket::setTCPSocket(ip::tcp::socket *s) {
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

TAwaitVoid TCPSocket::co_start() {
    co_await Socket::co_start();
    setNagle(false);
    setKeepAlive(true);
    co_return;
}

TAwaitVoid TCPSocket::co_close() {
    forceClose(*m_socket);
    co_return;
}

TAwaitSize TCPSocket::co_readSome(uint8_t *ptr, const std::size_t& size) {
    co_return co_await m_socket->async_read_some(buffer(ptr, size), use_awaitable);
}

TAwaitVoid TCPSocket::co_readAll(uint8_t* ptr, const std::size_t& size) {
    co_await async_read(*m_socket, buffer(ptr, size), use_awaitable);
}

TAwaitVoid TCPSocket::co_writeAll(const uint8_t* ptr, const std::size_t &size) {
    co_await async_write(*m_socket, buffer(ptr, size), use_awaitable);
}

void TCPSocket::cancel() {
    debug_print_this("");
    boost::system::error_code ec;
    m_socket->cancel(ec);
}

Socket* TCPSocket::create(boost::asio::io_context& io) {
    return new TCPSocket(io);
}

void TCPSocket::forceClose(TSocket& sock) {
    boost::system::error_code ec1, ec2;
    sock.shutdown(ip::tcp::socket::shutdown_both, ec1);
    sock.close(ec2);
}
