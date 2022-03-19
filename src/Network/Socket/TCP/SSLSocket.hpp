#ifndef SSL_SOCKET_H
#define SSL_SOCKET_H

#include <boost/asio/ssl.hpp>

#include "TCPSocket.hpp"

class SSLSocket : public TCPSocket {
public:
    SSLSocket(boost::asio::io_context &io);
    SSLSocket(boost::asio::io_context &io, boost::asio::ip::tcp::socket&& socket);
    virtual ~SSLSocket();

    void setClient(bool client);

    void setSSLParameters(const std::string& verifyHost,
                          const std::string& keysPath = "",
                          const std::string& entityName = "entity");
    void setSSL(bool);

private:
    Socket* create(boost::asio::io_context &io) override;

    void async_read_some(uint8_t*, const std::size_t&, const TDataCallback&) override;
    void async_read_all(uint8_t*, const std::size_t&, const TDataCallback&) override;

    void async_write_all(const uint8_t*, const std::size_t&, const TDataCallback&) override;

    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> TSocket;
    typedef boost::asio::ssl::context TContext;

    void asyncStartImpl(const TErrorCallback&) override;
    void async_close(const TErrorCallback&) override;

    bool m_ssl;
    bool m_client;
    std::unique_ptr<TContext> m_sslContext;
    std::unique_ptr<TSocket> m_socket;
};

#endif // SSL_SOCKET_H
