#ifndef SOCKET_H
#define SOCKET_H

#include "Coroutine/Awaitables.hpp"
#include "Misc/BaseFactoryObject.hpp"
#include "Misc/IOContextHolder.hpp"

class Socket : public BaseFactoryObject, public IOContextHolder {
public:
    Socket(boost::asio::io_context&);
    ~Socket();

    TAwaitVoid co_start();
    TAwaitVoid co_close();

    virtual TAwaitSize co_readSome(uint8_t*, const std::size_t&) = 0;
    virtual TAwaitSize co_readAll(uint8_t*, const std::size_t&) = 0;

    virtual TAwaitSize co_writeAll(const uint8_t*, const std::size_t&) = 0;

    virtual Socket* create(boost::asio::io_context&) = 0;

protected:
    virtual TAwaitVoid start() = 0;
    virtual TAwaitVoid close() = 0;

private:
    enum class State {
        INITIAL,
        STARTED,
        CLOSED
    };

    State m_state;
};

#endif // SOCKET_H
