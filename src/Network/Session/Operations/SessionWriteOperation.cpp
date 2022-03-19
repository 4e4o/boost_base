#include "SessionWriteOperation.hpp"
#include "Network/Session/Session.hpp"
#include "Network/Socket/Socket.hpp"

using boost::system::error_code;

static uint8_t* createCopy(const uint8_t *ptr, std::size_t size) {
    uint8_t *newPtr = new uint8_t[size];
    std::memcpy(newPtr, ptr, size);
    return newPtr;
}

SessionWriteOperation::SessionWriteOperation(const uint8_t *ptr, std::size_t size, bool copy, TSession s)
    : SessionOperation(s, s->getTimeout(Session::Timeouts::WRITE)),
      m_ptr(copy ? (createCopy(ptr, size)) : ptr),
      m_size(size), m_copy(copy) {
}

SessionWriteOperation::~SessionWriteOperation() {
    if (m_copy)
        delete[] m_ptr;
}

void SessionWriteOperation::startImpl() {
    SessionOperation::startImpl();
    Socket* sock = socket();

    sock->async_write_all(m_ptr, m_size,
                          [this](const error_code& ec,
                          const std::size_t& transferred) {
        finish(!ec && (m_size == transferred));
    });
}
