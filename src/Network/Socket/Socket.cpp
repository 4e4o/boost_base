#include "Socket.hpp"

Socket::Socket(boost::asio::io_context& io)
    : IOContextHolder(io),
      m_state(State::INITIAL) {
}

Socket::~Socket() {
}

TAwaitVoid Socket::co_start() {
    if (m_state != State::INITIAL)
        co_return;

    m_state = State::STARTED;
    co_return co_await start();
}

TAwaitVoid Socket::co_close() {
    if (m_state != State::STARTED)
        co_return;

    m_state = State::CLOSED;
    co_return co_await close();

}
