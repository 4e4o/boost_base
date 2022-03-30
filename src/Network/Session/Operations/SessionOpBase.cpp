#include "SessionOpBase.hpp"

SessionOpBase::SessionOpBase(Session* s)
    : m_session(s),
      m_buffer(DEFAULT_BUFFER_SIZE) {
}

SessionOpBase::~SessionOpBase() {
}

const uint8_t* SessionOpBase::ptr() const {
    return m_buffer.data();
}

uint8_t* SessionOpBase::ptr() {
    return m_buffer.data();
}

void SessionOpBase::setBufferSize(const std::size_t& s) {
    m_buffer.resize(s);
}

SessionOpBase::TBuffer& SessionOpBase::buffer() {
    return m_buffer;
}

Session* SessionOpBase::session() const {
    return m_session;
}
