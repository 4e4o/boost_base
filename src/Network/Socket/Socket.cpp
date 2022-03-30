#include "Socket.hpp"

Socket::Socket(boost::asio::io_context& io)
    : IOContextHolder(io),
      m_started(false) {
}

Socket::~Socket() {
}

bool Socket::started() const {
    return m_started;
}

TAwaitVoid Socket::co_start() {
    m_started = true;
    co_return;
}
