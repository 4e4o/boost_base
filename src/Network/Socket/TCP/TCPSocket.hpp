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

    static void forceClose(TSocket&);

protected:
    TAwaitVoid co_start() override;
    TAwaitVoid co_close() override;

    TAwaitSize co_readSome(uint8_t*, const std::size_t&) override;
    TAwaitVoid co_readAll(uint8_t*, const std::size_t&) override;

    TAwaitVoid co_writeAll(const uint8_t*, const std::size_t&) override;

    void cancel() override;

    void setTCPSocket(boost::asio::ip::tcp::socket*);

private:
    TCPSocket(boost::asio::io_context&);

    std::unique_ptr<TSocket> m_socket;
    bool m_socketOwner;
};

#endif // TCP_SOCKET_H
