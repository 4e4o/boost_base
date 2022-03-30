#ifndef SOCKET_H
#define SOCKET_H

#include "Coroutine/Awaitables.hpp"
#include "Misc/BaseFactoryObject.hpp"
#include "Misc/IOContextHolder.hpp"

class Socket : public BaseFactoryObject, public IOContextHolder {
public:
    Socket(boost::asio::io_context&);
    ~Socket();

    bool started() const;

    virtual TAwaitVoid co_start();
    virtual TAwaitVoid co_close() = 0;

    virtual TAwaitSize co_readSome(uint8_t*, const std::size_t&) = 0;
    virtual TAwaitVoid co_readAll(uint8_t*, const std::size_t&) = 0;

    virtual TAwaitVoid co_writeAll(const uint8_t*, const std::size_t&) = 0;

    // cancel all async operations
    virtual void cancel() = 0;

    virtual Socket* create(boost::asio::io_context&) = 0;

private:
    bool m_started;
};

#endif // SOCKET_H
