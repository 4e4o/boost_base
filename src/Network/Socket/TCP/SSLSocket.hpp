#ifndef SSL_SOCKET_H
#define SSL_SOCKET_H

#include <boost/asio/ssl.hpp>

#include "TCPSocket.hpp"

class SSLSocket : public TCPSocket {
public:
    SSLSocket(boost::asio::io_context&, boost::asio::ip::tcp::socket&&);
    virtual ~SSLSocket();

    void setClient(bool client);

    void setSSLParameters(const std::string& verifyHost,
                          const std::string& keysPath = "",
                          const std::string& entityName = "entity");
    void setSSL(bool);

private:
    SSLSocket(boost::asio::io_context&);
    Socket* create(boost::asio::io_context&) override;

    TAwaitVoid co_start() override;
    TAwaitVoid co_close() override;

    TAwaitSize co_readSome(uint8_t*, const std::size_t&) override;
    TAwaitVoid co_readAll(uint8_t*, const std::size_t&) override;

    TAwaitVoid co_writeAll(const uint8_t*, const std::size_t&) override;

    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> TSocket;
    typedef boost::asio::ssl::context TContext;

    bool m_ssl;
    bool m_client;
    std::unique_ptr<TContext> m_sslContext;
    std::unique_ptr<TSocket> m_socket;
};

#endif // SSL_SOCKET_H
