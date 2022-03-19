#include "Session.hpp"
#include "Misc/Debug.hpp"
#include "Misc/Timer.hpp"
#include "Network/Socket/Socket.hpp"
#include "Operations/OperationsDispatcher.hpp"
#include "Operations/SessionCloseOperation.hpp"
#include "Operations/SessionStartOperation.hpp"
#include "Operations/SessionReadOperation.hpp"
#include "Operations/SessionWriteOperation.hpp"
#include "Operations/SessionReader.hpp"
#include "Operations/SessionWriter.hpp"

#define START_TIMEOUT   TSeconds(10)
#define CLOSE_TIMEOUT   TSeconds(5)

#define SESSION_GETTER  [this]() -> TSession { return shared_from_this<Session>(); }

#ifndef NDEBUG
#define DEBUG_SESSIONS_COUNT    true
#endif // NDEBUG

using boost::signals2::connection;

using StartOp = SessionStartOperation;
using CloseOp = SessionCloseOperation;
using ReadOp = SessionReadOperation;
using WriteOp = SessionWriteOperation;

#ifdef DEBUG_SESSIONS_COUNT
static std::atomic<int> g_debugSessionsCount(0);
#endif

Session::Session(Session *s)
    : Session(s->releaseSocket()) {
}

Session::Session(Socket *s)
    : StrandHolder(s->io()),
      m_state(State::Initial),
      m_closeOnStart(false),
      m_closeOpStarted(false),
      m_allowInactivity(false),
      m_readOps(new OperationsDispatcher()),
      m_writeOps(new OperationsDispatcher()),
      m_stateOps(new OperationsDispatcher()),
      m_reader(new SessionReader(SESSION_GETTER, m_readOps.get())),
      m_writer(new SessionWriter(SESSION_GETTER, m_writeOps.get())),
      m_allowInactivityTimer(new Timer(io())) {
    debug_print(boost::format("Session::Session %1%") % this);

    debug_print(boost::format("Session::Session %1% disps: %2% %3% %4%") % this % m_readOps.get() % m_writeOps.get() % m_stateOps.get());


    setSocket(s);
    setTimeout(Timeouts::START, START_TIMEOUT);
    setTimeout(Timeouts::CLOSE, CLOSE_TIMEOUT);

    m_allowInactivityTimer->setStrand(this);

#ifdef DEBUG_SESSIONS_COUNT
    g_debugSessionsCount++;
    debug_print(boost::format("Sessions::Sessions count %1%") % g_debugSessionsCount);
#endif // DEBUG_SESSIONS_COUNT
}

Session::~Session() {
    assert(m_state == State::Closed);

#ifdef DEBUG_SESSIONS_COUNT
    g_debugSessionsCount--;
    debug_print(boost::format("Session::~Session count %1% ") % g_debugSessionsCount);
#endif // DEBUG_SESSIONS_COUNT
}

Socket* Session::releaseSocket() {
    // FIXME !
    // stopped state
    Socket *sock = m_socket.release();
    setSocket(sock->create(io()));
    return sock;
}

void Session::disconnectAllSlots() {
    // FIXME ?
/*    starting.disconnect_all_slots();
    started.disconnect_all_slots();
    closing.disconnect_all_slots();
    closed.disconnect_all_slots();*/
}

void Session::setSocket(Socket* s) {
    m_socket.reset(s);
    s->setStrand(this);
}

SessionReader& Session::reader() const {
    return *m_reader;
}

SessionWriter& Session::writer() const {
    return *m_writer;
}

TTimeDuration Session::getTimeout(Timeouts t) {
    if (m_timeouts.contains(t))
        return m_timeouts[t];

    return boost::none;
}

void Session::setTimeout(Timeouts t, const TTimeDuration& d) {
    if (d.has_value()) {
        m_timeouts[t] = d;
    } else {
        m_timeouts.erase(t);
    }
}

Session::State Session::state() const {
    STRAND_ASSERT(this);
    return m_state;
}

void Session::start() {
    post<true>([this] {
        doStart();
    });
}

void Session::doStart() {
    STRAND_ASSERT(this);
    assert(m_socket != nullptr);

    if (m_state != State::Initial)
        return;

    m_activeSelf = shared_from_this<Session>();
    m_state = State::Starting;
    starting();

    m_startOp.reset(new StartOp(m_activeSelf, getTimeout(Timeouts::START)));
    m_closeOp.reset(new CloseOp(m_activeSelf, getTimeout(Timeouts::CLOSE)));

    m_startOp->completed.connect([this](bool success) {
        STRAND_ASSERT(this);

        if (m_state != State::Starting || !success)
            return;

        m_state = State::Started;
        started();
        startImpl();
        trackDispatcher(m_writeOps.get());
        trackDispatcher(m_readOps.get());
        m_writeOps->start();
        m_readOps->start();
    });

    m_closeOp->starting.connect([this] {
        STRAND_ASSERT(this);
        assert(m_state == State::Closing);
        m_closeOpStarted = true;
    });

    m_closeOp->completed.connect([this](auto) {
        STRAND_ASSERT(this);
        assert(m_state == State::Closing);
        m_state = State::Closed;
        closed();
        disconnectAllSlots();
        // отпускаем себя только когда закрыты
        post<true>([this] {
            m_activeSelf.reset();
        });
    });

    initAllowInactivityTimer();
    trackDispatcher(m_stateOps.get());
    m_stateOps->pushOp(m_startOp.get());
    m_stateOps->start();

    if (m_closeOnStart)
        close();
}

void Session::startImpl() {
}

void Session::trackDispatcher(OperationsDispatcher* d) {
    STRAND_ASSERT(this);
    d->inactive.connect([this] {
//        debug_print(boost::format("Session::inactive %1%") % this);
        checkDispatchersInactivity();

    //    debug_print(boost::format("Session::inactive post2 %1%") % this);
    });
    d->opError.connect([this](auto) {
        debug_print(boost::format("Session %1% operation error, closing %2%") % this % socket());
        close();
    });
    d->finished.connect([this] { checkStartCloseOp(); });
    d->opAdded.connect([this](auto) { stopAllowInactivity(); });
}

void Session::wait(const TTimeDuration& d) {
//    debug_print(boost::format("Session::wait1 %1%") % this);

    m_lastWaiTime = boost::posix_time::second_clock::local_time();
    m_lastDUr = d;

    dispatch<true>([this, d] {
        startAllowInactivity(d);
    });

//    debug_print(boost::format("Session::wait2 %1%") % this);
}

void Session::initAllowInactivityTimer() {
    m_allowInactivityTimer->timeout.connect([this](auto) {
        m_allowInactivity = false;
        checkDispatchersInactivity();
    });

    m_allowInactivityTimer->lockWhenActive(weak_from_this());
}

void Session::startAllowInactivity(const TTimeDuration& d) {
//    debug_print(boost::format("Session::startAllowInactivity %1% %2%") % this % (int) m_state);

    // не надо юзать wait() перед стартом, он будет отменён!
    assert(m_state != State::Initial);

    if (m_state != State::Started)
        return;

    m_allowInactivity = true;
    m_allowInactivityTimer->stopTimer();
    m_allowInactivityTimer->startTimer(d);
}

void Session::stopAllowInactivity() {
//    debug_print(boost::format("Session::stopAllowInactivity %1% %2% %3%") % this % (int) m_state % m_allowInactivity);

    if (!m_allowInactivity)
        return;

    m_lastWaiTime = boost::none;
    m_allowInactivity = false;
    m_allowInactivityTimer->stopTimer();
}

void Session::close() {
    post<true>([this] {
        doClose(false);
    });
}

void Session::closeWaitWrite() {
    post<true>([this] {
        doClose(true);
    });
}

void Session::doClose(bool waitWrite) {
    STRAND_ASSERT(this);

    if (m_state == State::Initial) {
        // мы еще не запущены, закрыться не можем
        m_closeOnStart = true;
        return;
    }

    if ((m_state == State::Closing) || (m_state == State::Closed))
        return;

    debug_print(boost::format("Session::doClose %1%") % this);

    m_state = State::Closing;
    closing();

    stopAllowInactivity();

    // не запускаем новые операции
    m_writeOps->stop(!waitWrite);
    m_readOps->stop(true);
    m_stateOps->stop(true);

    // FIXME
    // тут для waitWrite надо ждать пока всё завершится
    // и только потом делать socket()->cancel();
    // но! в любом случае надо делать socket()->cancel потому что
    // нам надо чтения остановить
    socket()->cancel();

    // теперь надо ждать когда все диспетчеры остановятся
    // после этого надо выполнить m_closeOp

    checkStartCloseOp();
}

void Session::checkStartCloseOp() {
    STRAND_ASSERT(this);

    if (m_state != State::Closing || m_closeOpStarted)
        return;

    // запускаем операцию закрытия только когда все диспетчеры завершены
    // т.е. не имеют активных операций и остановлены

    if (!m_writeOps->isFinished() || !m_readOps->isFinished() ||
            !m_stateOps->isFinished())
        return;

    m_closeOp->start();
}

void Session::checkDispatchersInactivity() {
    STRAND_ASSERT(this);

    if (m_state != State::Started)
        return;

    if (!m_writeOps->isInactive() || !m_readOps->isInactive() ||
            !m_stateOps->isInactive())
        return;

    if (m_allowInactivity) {
      //  debug_print(boost::format("Session::checkDispatchersInactivity %1% becomes inactive, allowInactivity is set") % this);
        return;
    } else {
        if (m_lastWaiTime.has_value()) {
    //        debug_print(boost::format("Session::checkDispatchersInactivity %1% becomes inactive, closing, check validity") % this);

            auto t1 = boost::posix_time::second_clock::local_time();
            auto diff = t1 - m_lastWaiTime.value();
            int secs = diff.seconds() - m_lastDUr.value()->seconds();

            assert(secs >= 0);

            if (secs < 0) {
                debug_print(boost::format("Session::checkDispatchersInactivity %1% not ok") % this);
            } else {
//                debug_print(boost::format("Session::checkDispatchersInactivity %1% ok") % this);
            }

        }

        debug_print(boost::format("Session::checkDispatchersInactivity %1% becomes inactive, closing") % this);
    }

    close();
}
