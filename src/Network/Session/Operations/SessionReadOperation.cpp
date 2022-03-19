#include "SessionReadOperation.hpp"
#include "Network/Session/Session.hpp"
#include "Network/Socket/Socket.hpp"
#include "Misc/Debug.hpp"

using boost::system::error_code;

SessionReadOperation::SessionReadOperation(uint8_t *ptr, std::size_t size, bool all, TSession s)
    : SessionOperation(s, s->getTimeout(Session::Timeouts::READ)),
      m_ptr(ptr), m_size(size), m_all(all) {
}

void SessionReadOperation::startImpl() {
    SessionOperation::startImpl();
    Socket* sock = socket();

    if (m_all) {
        sock->async_read_all(m_ptr, m_size,
                             [this](const error_code &ec,
                             const std::size_t& transferred) {
            // debug_print(boost::format("SessionReadOperation read_all error %1%") % ec.message());
            m_readed = transferred;
            finish(!ec && (m_size == transferred));
        });
    } else {
        sock->async_read_some(m_ptr, m_size,
                              [this](const error_code &ec,
                              const std::size_t& transferred) {
            //            debug_print(boost::format("SessionReadOperation read_some error %1%") % ec.message());
            m_readed = transferred;
            finish(!ec);
        });
    }
}

std::size_t SessionReadOperation::readed() const {
    return m_readed;
}
