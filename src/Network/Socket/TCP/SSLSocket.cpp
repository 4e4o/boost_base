#include "SSLSocket.hpp"
#include "Misc/Debug.hpp"

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

using handshake_type = boost::asio::ssl::stream_base::handshake_type;
using error = boost::system::error_code;
using boost::asio::ip::tcp;

static boost::asio::ssl::context* createSSLContext(const std::string& keysPath = "",
                                                   const std::string& entityName = "entity") {
    using namespace boost::asio::ssl;

    context* ctx = new context(boost::asio::ssl::context::tls);
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

SSLSocket::SSLSocket(boost::asio::io_context &io)
    : SSLSocket(io, tcp::socket(io)) {
}

SSLSocket::SSLSocket(boost::asio::io_context &io, tcp::socket&& socket)
    : TCPSocket(io),
      m_ssl(true),
      m_client(true),
      m_sslContext(new boost::asio::ssl::context(boost::asio::ssl::context::tls)),
      m_socket(new TSocket(std::move(socket), *m_sslContext)) {
    setTCPSocket(&m_socket->next_layer());
    debug_print(boost::format("SSLSocket::SSLSocket %1%") % this);
}

SSLSocket::~SSLSocket() {
    debug_print(boost::format("SSLSocket::~SSLSocket %1%") % this);
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

Socket* SSLSocket::create(boost::asio::io_context &io) {
    return new SSLSocket(io);
}

void SSLSocket::asyncStartImpl(const TErrorCallback& c) {
    const handshake_type type = m_client ? handshake_type::client : handshake_type::server;

    m_socket->async_handshake(type, bindExecutor([this, c] (const error& ec) {
        debug_print(boost::format("initSSL async_handshake error = %1%, %2%") % ec.message().c_str() % ec);

        if (ec) {
            c(ec);
            return;
        }

        TCPSocket::asyncStartImpl(c);
    }));
}

void SSLSocket::async_read_some(uint8_t* ptr, const std::size_t &size, const TDataCallback &c) {
    if (m_ssl)
        m_socket->async_read_some(boost::asio::buffer(ptr, size), bindExecutor(c));
    else
        TCPSocket::async_read_some(ptr, size, c);
}

void SSLSocket::async_read_all(uint8_t* ptr, const std::size_t &size, const TDataCallback& c) {
    if (m_ssl)
        boost::asio::async_read(*m_socket, boost::asio::buffer(ptr, size), bindExecutor(c));
    else
        TCPSocket::async_read_all(ptr, size, c);
}

void SSLSocket::async_write_all(const uint8_t *ptr, const std::size_t &size, const TDataCallback &c) {
    if (m_ssl)
        boost::asio::async_write(*m_socket, boost::asio::buffer(ptr, size), bindExecutor(c));
    else
        TCPSocket::async_write_all(ptr, size, c);
}

void SSLSocket::async_close(const TErrorCallback& c) {
    if (m_ssl) {
        cancel();
        debug_print(boost::format("SSLSocket::async_shutdown start %1%") % this);
        m_socket->async_shutdown(bindExecutor([this, c](const error& ec) {
            debug_print(boost::format("SSLSocket::async_shutdown done %1% %2%") % this % ec);
            TCPSocket::async_close(c);
        }));
    } else {
        TCPSocket::async_close(c);
    }
}
