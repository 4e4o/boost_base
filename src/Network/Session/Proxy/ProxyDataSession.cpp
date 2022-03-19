#include "ProxyDataSession.hpp"
#include "Network/Session/Operations/SessionReader.hpp"
#include "Network/Session/Operations/SessionWriter.hpp"
#include "Misc/Timer.hpp"
#include "Misc/Debug.hpp"

#define WAIT_OTHER_TIMEOUT TSeconds(10)

// FIXME increase read buffer ?

ProxyDataSession::ProxyDataSession(Session* s)
    : Session(s) {
    debug_print(boost::format("ProxyDataSession::ProxyDataSession %1% from Session = %2%") % this % s);
}

ProxyDataSession::~ProxyDataSession() {
    debug_print(boost::format("ProxyDataSession::~ProxyDataSession %1%") % this);
}

void ProxyDataSession::setOther(const TSessionPtr &s) {
    m_other = s;

    closed.connect([this] {
        m_other->closeWaitWrite();
        m_other.reset();
    });
}

void ProxyDataSession::startImpl() {
    debug_print(boost::format("ProxyDataSession::startImpl 1 %1%") % this);

    reader()([this](const uint8_t *ptr, std::size_t size) {
        assert(m_other != nullptr);

//        debug_print(boost::format("ProxyDataSession::reader %1% %2%") % this % size);

        wait(WAIT_OTHER_TIMEOUT);
        // m_other тут жив всегда потому что this еще не закрыт
        // похуй что m_other может быть закрыт
        m_other->writer()([this] {
            // это значит мы закрылись
            if (m_other == nullptr)
                return;

 //           debug_print(boost::format("ProxyDataSession::writer %1% to %2%") % this % m_other.get());

            m_other->wait(WAIT_OTHER_TIMEOUT);
            reader().repeat();
        }, ptr, size);
    });

    debug_print(boost::format("ProxyDataSession::startImpl 2 %1%") % this);
}

ProxyDataSession::TSessionPtr ProxyDataSession::proxy(std::shared_ptr<Session> s1, std::shared_ptr<Session> s2) {
    debug_print(boost::format("ProxyDataSession::proxy %1% %2%") % s1.get() % s2.get());

    // FIXME !! stop s1 and s2

    std::shared_ptr<ProxyDataSession> p1(new ProxyDataSession(s1.get()));
    std::shared_ptr<ProxyDataSession> p2(new ProxyDataSession(s2.get()));
    p1->setOther(p2);
    p2->setOther(p1);
    p1->start();
    p2->start();
/*
    Timer::TTimerPtr timer(new Timer(s1->io(), TSeconds(2)));

    timer->timeout.connect_extended([p1, timer](auto c, auto) {
        c.disconnect();
        p1->start();
    });

    timer->startTimer();
*/
    s1->close();
    s2->close();
    return p1;
}
