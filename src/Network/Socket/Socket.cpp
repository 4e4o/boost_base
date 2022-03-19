#include "Socket.hpp"

Socket::Socket(boost::asio::io_context &io)
    : StrandHolder(io),
      m_started(false) {
}

Socket::~Socket() {
}

bool Socket::started() const {
    STRAND_ASSERT(this);
    return m_started;
}

void Socket::async_start(const TErrorCallback& c) {
    asyncStartImpl([this, c] (const boost::system::error_code& ec) {
        if (!ec)
            m_started = true;

        c(ec);
    });
}
