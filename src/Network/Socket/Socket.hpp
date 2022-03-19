#ifndef SOCKET_H
#define SOCKET_H

#include "Misc/StrandHolder.hpp"

class Socket : public StrandHolder {
public:
    Socket(boost::asio::io_context &io);
    virtual ~Socket();

    typedef std::function<void(const boost::system::error_code&, const std::size_t&)> TDataCallback;
    typedef std::function<void(const boost::system::error_code&)> TErrorCallback;

    bool started() const;

    void async_start(const TErrorCallback&);
    virtual void async_close(const TErrorCallback&) = 0;

    virtual void async_read_some(uint8_t*, const std::size_t&, const TDataCallback&) = 0;
    virtual void async_read_all(uint8_t*, const std::size_t&, const TDataCallback&) = 0;

    virtual void async_write_all(const uint8_t*, const std::size_t&, const TDataCallback&) = 0;

    // cancel all async operations
    virtual void cancel() = 0;

    virtual Socket* create(boost::asio::io_context &io) = 0;

protected:
    virtual void asyncStartImpl(const TErrorCallback&) = 0;

private:
    bool m_started;
};

#endif // SOCKET_H
