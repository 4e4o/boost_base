#include "StrandHolder.hpp"

StrandHolder::StrandHolder(boost::asio::io_context &io)
    : m_io(io),
      m_strand(new boost::asio::io_context::strand(io)),
      m_owner(true) {
}

StrandHolder::~StrandHolder() {
    if (!m_owner)
        m_strand.release();
}

boost::asio::io_context::strand& StrandHolder::strand() {
    return *m_strand.get();
}

boost::asio::io_context &StrandHolder::io() {
    return m_io;
}

void StrandHolder::setStrand(StrandHolder* s, bool owner) {
    setStrand(&(s->strand()), owner);
}

void StrandHolder::setStrand(boost::asio::io_context::strand *s, bool owner) {
    if (!m_owner)
        m_strand.release();

    m_strand.reset(s);
    m_owner = owner;
}
