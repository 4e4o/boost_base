#include "SessionOperation.hpp"
#include "Network/Session/Session.hpp"
#include "Network/Socket/Socket.hpp"
#include "Misc/Timer.hpp"
#include "Misc/Debug.hpp"

SessionOperation::SessionOperation(TSession s, const TTimeDuration& d)
    : Operation(s->io()),
      m_timeout(new Timer(s->io(), d)),
      m_weakSession(s) {
    setStrand(s.get());
    m_timeout->setStrand(this);
    m_timeout->timeout.connect([this](auto) {
        socket()->cancel();
    });
    // фиксит краш в лямбде m_timeout->timeout наверху
    m_timeout->lockWhenActive(m_weakSession);
}

SessionOperation::~SessionOperation() {
}

void SessionOperation::startImpl() {
    m_timeout->startTimer();
}

void SessionOperation::finish(bool success) {
    m_timeout->stopTimer();
    completed(success);
}

SessionOperation::TSession SessionOperation::session() const {
    auto session = m_weakSession.lock();
    assert(session != nullptr);
    return session;
}

Socket* SessionOperation::socket() const {
    return session()->socket();
}
