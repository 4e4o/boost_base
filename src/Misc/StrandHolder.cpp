#include "StrandHolder.hpp"

StrandHolder::StrandHolder(boost::asio::io_context &io)
    : IOContextHolder(io),
      m_executor(new TStrandExecutor(boost::asio::make_strand(io))),
      m_other(nullptr) {
}

StrandHolder::~StrandHolder() {
}

void StrandHolder::setStrand(StrandHolder* s) {
    m_other = s;
    m_executor.reset();
}

TStrandExecutor& StrandHolder::executor() const {
    if (m_other != nullptr)
        return m_other->executor();

    return *m_executor.get();
}

bool StrandHolder::running_in_strand_thread() const {
    return executor().running_in_this_thread();
}
