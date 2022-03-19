#include "OperationsDispatcher.hpp"
#include "Operation.hpp"
#include "Misc/Debug.hpp"

using boost::signals2::connection;

OperationsDispatcher::OperationsDispatcher()
    : m_state(State::INITIAL),
      m_currentOp(nullptr)/*,
      m_postCounter(new bool())*/ {
}

OperationsDispatcher::~OperationsDispatcher() {
}

void OperationsDispatcher::start() {
    if (m_state != State::INITIAL)
        return;

    m_state = State::RUNNING;
    processNext();
}

void OperationsDispatcher::processNext() {
    if ((m_state != State::RUNNING) || (m_currentOp != nullptr))
        return;

    if (!m_operations.empty()) {
        m_currentOp = m_operations.front();
        m_operations.pop_front();

        m_currentOp->completed.connect_extended([this](const connection &c, bool success) {
            c.disconnect();
            onOpResult(success);
        });

        m_currentOp->start();
    }
}

void OperationsDispatcher::onOpResult(bool success) {
    assert(m_currentOp != nullptr);
    Operation *savedOp = m_currentOp;
    m_currentOp = nullptr;

    checkMoveToFinished();

    if (isInactive()) {
        inactive();
    }

    if (!success) {
        opError(savedOp);
    }

    processNext();
}

void OperationsDispatcher::stop(bool force) {
    if ((m_state == State::STOPPED) || (m_state == State::FINISHED))
        return;

    if (force)
        clear();

    m_state = State::STOPPED;
    checkMoveToFinished();
}

void OperationsDispatcher::checkMoveToFinished() {
    if (m_state != State::STOPPED)
        return;

    if (m_currentOp == nullptr) {
        m_state = State::FINISHED;
        finished();
    }
}

void OperationsDispatcher::pushOp(Operation *op) {
//    debug_print(boost::format("OperationsDispatcher::pushOp %1% %2%") % this % op);
    m_operations.push_back(op);
    opAdded(op);
    processNext();
}

void OperationsDispatcher::clear() {
    while(!m_operations.empty()) {
        m_operations.pop_front();
    }
}

bool OperationsDispatcher::isFinished() const {
    return m_state == State::FINISHED;
}

bool OperationsDispatcher::isInactive() const {
    const bool res = (m_currentOp == nullptr) && m_operations.empty()/* && (m_postCounter.use_count() <= 1)*/;
//    debug_print(boost::format("OperationsDispatcher::isInactive %1% %2% %3%") % this % m_postCounter.use_count() % res);
    return res;
}
