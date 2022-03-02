#include "TCPSocket.hpp"
#include "AApplication.h"
#include "Misc/Debug.hpp"

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

TCPSocket::TCPSocket(boost::asio::io_context &io)
    : TCPSocket(boost::asio::ip::tcp::socket(io)) {
}

TCPSocket::TCPSocket(TCPSocket&& socket)
    : TCPSocket(std::move(socket.m_socket->next_layer())) {
}

TCPSocket::TCPSocket(boost::asio::ip::tcp::socket&& socket)
    : m_ssl(false),
      m_sslContext(new boost::asio::ssl::context(boost::asio::ssl::context::tls)),
      m_socket(new TSocket(std::move(socket), *m_sslContext)) {
}

TCPSocket::~TCPSocket() {
}

void TCPSocket::init() {
    using boost::asio::ip::tcp;
    using boost::asio::socket_base;
    using boost::asio::detail::socket_option::integer;

    auto& socket = m_socket->next_layer();

    socket.set_option(tcp::no_delay(true));
    socket.set_option(socket_base::keep_alive(true));
    socket.set_option(integer<SOL_TCP, TCP_KEEPIDLE>(20));      // secs before keepalive probes
    socket.set_option(integer<SOL_TCP, TCP_KEEPINTVL>(10));     // interval between keepalive
    socket.set_option(integer<SOL_TCP, TCP_KEEPCNT>(4));        // failed keepalive before declaring dead
}

void TCPSocket::close() {
    if (m_ssl) {
        try {
            m_socket->shutdown();
        } catch(...) { }
    }

    try {
        m_socket->next_layer().close();
    } catch(...) { }
}

void TCPSocket::setSSLParameters(const std::string& verifyHost,
                                 const std::string& keysPath,
                                 const std::string& entityName) {
    using namespace boost::asio::ssl;

    m_sslContext.reset(createSSLContext(keysPath, entityName));
    m_socket.reset(new TSocket(std::move(m_socket->next_layer()), *m_sslContext));

#if BOOST_VERSION < 107300
    m_socket->set_verify_callback(rfc2818_verification(verifyHost));
#else
    m_socket->set_verify_callback(host_name_verification(verifyHost));
#endif // BOOST_VERSION < 107300
}

void TCPSocket::initSSL(boost::asio::ssl::stream_base::handshake_type type,
                        TInitResult r) {
    using namespace boost::asio::ssl;
    using namespace boost::system;

    m_ssl = true;
    m_socket->set_verify_mode(verify_peer);

    m_socket->async_handshake(type, [r] (const error_code& error) {
        debug_print(boost::format("initSSL async_handshake error = %1%") % error.message().c_str());
        r(!error);
    });
}

void TCPSocket::setSSL(bool enabled) {
    m_ssl = enabled;
}
