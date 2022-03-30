#include "SSLSocket.hpp"
#include "Misc/Debug.hpp"

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/use_awaitable.hpp>

using handshake_type = boost::asio::ssl::stream_base::handshake_type;
using error = boost::system::error_code;
using boost::asio::ip::tcp;
using namespace boost::asio;

static ssl::context* createSSLContext(const std::string& keysPath = "",
                                      const std::string& entityName = "entity") {
    using namespace boost::asio::ssl;

    context* ctx = new context(context::tls);
    ctx->set_options(context::default_workarounds
                     | context::no_sslv2
                     | context::no_sslv3
                     | context::no_tlsv1
                     | context::no_tlsv1_1);

    ctx->load_verify_file(keysPath + "ca.crt");
    ctx->use_certificate_file(keysPath + entityName + ".crt", context::pem);
    ctx->use_private_key_file(keysPath + entityName + ".key", context::pem);
    return ctx;
}

SSLSocket::SSLSocket(io_context &io)
    : SSLSocket(io, tcp::socket(io)) {
}

SSLSocket::SSLSocket(io_context &io, tcp::socket&& socket)
    : TCPSocket(io, tcp::socket(io)),
      m_ssl(true),
      m_client(true),
      m_sslContext(new ssl::context(ssl::context::tls)),
      m_socket(new TSocket(std::move(socket), *m_sslContext)) {
    setTCPSocket(&m_socket->next_layer());
    debug_print_this("");
}

SSLSocket::~SSLSocket() {
    debug_print_this("");
}

void SSLSocket::setSSLParameters(const std::string& verifyHost,
                                 const std::string& keysPath,
                                 const std::string& entityName) {
    using namespace boost::asio::ssl;

    m_sslContext.reset(createSSLContext(keysPath, entityName));

    m_socket.reset(new TSocket(std::move(m_socket->next_layer()), *m_sslContext));
    setTCPSocket(&m_socket->next_layer());

#if BOOST_VERSION < 107300
    m_socket->set_verify_callback(rfc2818_verification(verifyHost));
#else
    m_socket->set_verify_callback(host_name_verification(verifyHost));
#endif // BOOST_VERSION < 107300

    m_socket->set_verify_mode(verify_peer);
}

void SSLSocket::setClient(bool client) {
    m_client = client;
}

void SSLSocket::setSSL(bool enabled) {
    m_ssl = enabled;
}

Socket* SSLSocket::create(boost::asio::io_context& io) {
    return new SSLSocket(io);
}

TAwaitVoid SSLSocket::co_start() {
    co_await TCPSocket::co_start();
    const handshake_type type = m_client ? handshake_type::client : handshake_type::server;
    co_await m_socket->async_handshake(type, use_awaitable);
    co_return;
}

TAwaitVoid SSLSocket::co_close() {
    if (m_ssl) {
        cancel();
        debug_print_this("start");
        co_await m_socket->async_shutdown(use_awaitable);
    }

    co_await TCPSocket::co_close();
}

TAwaitSize SSLSocket::co_readSome(uint8_t* ptr, const std::size_t& size) {
    if (m_ssl)
        co_return co_await m_socket->async_read_some(buffer(ptr, size), use_awaitable);
    else
        co_return co_await TCPSocket::co_readSome(ptr, size);
}

TAwaitVoid SSLSocket::co_readAll(uint8_t* ptr, const std::size_t& size) {
    if (m_ssl)
        co_await async_read(*m_socket, buffer(ptr, size), use_awaitable);
    else
        co_await TCPSocket::co_readAll(ptr, size);
}

TAwaitVoid SSLSocket::co_writeAll(const uint8_t* ptr, const std::size_t& size) {
    if (m_ssl)
        co_await async_write(*m_socket, boost::asio::buffer(ptr, size), use_awaitable);
    else
        co_await TCPSocket::co_writeAll(ptr, size);
}
