#include "StrandHolder.hpp"

StrandHolder::StrandHolder(boost::asio::io_context &io)
    : m_io(io),
      m_strand(new boost::asio::io_context::strand(io)),
      m_other(nullptr) {
}

StrandHolder::~StrandHolder() {
}

boost::asio::io_context &StrandHolder::io() const {
    return m_io;
}

void StrandHolder::setStrand(StrandHolder* s) {
    m_other = s;
    m_strand.reset();
}

boost::asio::io_context::strand& StrandHolder::strand() const {
    if (m_other != nullptr)
        return m_other->strand();

    return *m_strand.get();
}

bool StrandHolder::running_in_strand_thread() const {
    return strand().running_in_this_thread();
}
