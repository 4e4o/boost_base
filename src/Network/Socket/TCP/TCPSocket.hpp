#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "Network/Socket/Socket.hpp"

#include <boost/asio/ip/tcp.hpp>

class TCPSocket : public Socket {
public:
    TCPSocket(boost::asio::io_context& io);
    TCPSocket(boost::asio::io_context& io, boost::asio::ip::tcp::socket&& socket);
    ~TCPSocket();

    void setNagle(bool);
    void setKeepAlive(bool);

    Socket* create(boost::asio::io_context &io) override;

protected:
    void asyncStartImpl(const TErrorCallback&) override;
    void async_close(const TErrorCallback&) override;

    void async_read_some(uint8_t*, const std::size_t&, const TDataCallback&) override;
    void async_read_all(uint8_t*, const std::size_t&, const TDataCallback&) override;

    void async_write_all(const uint8_t*, const std::size_t&, const TDataCallback&) override;

    void cancel() override;

    void setTCPSocket(boost::asio::ip::tcp::socket*);

private:
    typedef boost::asio::ip::tcp::socket TSocket;

    void force_close(const TErrorCallback&);

    std::unique_ptr<TSocket> m_socket;
    bool m_socketOwner;
};

#endif // TCP_SOCKET_H
