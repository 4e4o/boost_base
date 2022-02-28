#ifndef SESSION_H
#define SESSION_H

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include <boost/signals2.hpp>
#include <boost/array.hpp>

#include "Misc/EnableSharedFromThisVirtual.hpp"
#include "Misc/StrandHolder.hpp"
#include "Network/TCPSocket.hpp"

class Session : public enable_shared_from_this_virtual<Session>, public StrandHolder {
public:
    typedef std::function<void()> TEvent;

    Session(boost::asio::io_context &io);
    Session(boost::asio::io_context &io, TCPSocket&& sock);
    virtual ~Session();

    void start();
    void close();

    void startSSL(bool client, TEvent);

    virtual void readSome(std::size_t maxSize = READ_BUFFER_SIZE);
    virtual void readAll(std::size_t size);
    virtual void writeAll(const uint8_t *ptr, std::size_t size);

    TCPSocket& socket();

    boost::signals2::signal<void (const uint8_t *ptr, std::size_t size)> onData;
    boost::signals2::signal<void (const boost::system::error_code& ec)> onError;
    boost::signals2::signal<void ()> onWriteDone;
    boost::signals2::signal<void ()> onClose;

    boost::signals2::signal<void ()> onDestroy;

protected:
    virtual void startImpl();
    void closeOnWrite();

private:
    static constexpr int READ_BUFFER_SIZE = 64 * 2 * 1024;

    void errorHandler(const boost::system::error_code&);
    bool isDisconnect(const boost::system::error_code&);
    void doWrite(const uint8_t *ptr, std::size_t size);
    void disconnectAllSlots();

    TCPSocket m_socket;
    std::array<uint8_t, READ_BUFFER_SIZE> m_readBuffer;
    bool m_writing;
    bool m_closeOnWrite;
    bool m_closed;
};

#endif // SESSION_H
