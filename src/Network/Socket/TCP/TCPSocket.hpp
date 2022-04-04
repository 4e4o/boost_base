#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "Network/Socket/Socket.hpp"

#include <boost/asio/ip/tcp.hpp>

class TCPSocket : public Socket {
public:
    typedef boost::asio::ip::tcp::socket TSocket;

    TCPSocket(boost::asio::io_context&, boost::asio::ip::tcp::socket&&);
    ~TCPSocket();

    void setNagle(bool);
    void setKeepAlive(bool);

    Socket* create(boost::asio::io_context&) override;

    // disable nagle and enable keep alive
    // enabled by default
    void setFastPreset(bool fastPreset);

protected:
    TAwaitVoid start() override;
    TAwaitVoid close() override;

    TAwaitSize co_readSome(uint8_t*, const std::size_t&) override;
    TAwaitSize co_readAll(uint8_t*, const std::size_t&) override;

    TAwaitSize co_writeAll(const uint8_t*, const std::size_t&) override;

    void cancel();

    void setTCPSocket(boost::asio::ip::tcp::socket*);

private:
    TCPSocket(boost::asio::io_context&);

    std::unique_ptr<TSocket> m_socket;
    bool m_socketOwner;
    bool m_fastPreset;
};

#endif // TCP_SOCKET_H
