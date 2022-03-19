#include "SessionOpManager.hpp"
#include "Network/Session/Session.hpp"
#include "Operations/OperationsDispatcher.hpp"
#include "SessionReadOperation.hpp"
#include "SessionWriteOperation.hpp"
#include "Pools/TSObjPool1.hpp"

template<class T>
SessionOpManager<T>::SessionOpManager(const TSessionGetter &g, OperationsDispatcher* d)
    : m_curOp(nullptr),
      m_prevOp(nullptr),
      m_toRecycle(nullptr),
      m_sessionReceived(false),
      m_sessionInit(g),
      m_pool(new TPool()),
      m_dispatcher(d) {
}

template<class T>
SessionOpManager<T>::~SessionOpManager() {
}

template<class T>
std::shared_ptr<Session> SessionOpManager<T>::session() {
    if (!m_sessionReceived) {
        m_sessionReceived = true;
        m_session = m_sessionInit();
    }

    return m_session.lock();
}

template<class T>
void SessionOpManager<T>::registerNewOp(T* op) {
    op->completed.connect([op, this](bool) {
        if (m_toRecycle != nullptr) {
            m_toRecycle->recycle();
            m_toRecycle = nullptr;
        }

        if (op != m_prevOp) {
            m_toRecycle = op;
        }
    });
    op->setRecycablePool(m_pool.get());
}

template<class T>
template<typename... Args>
typename SessionOpManager<T>::TOp* SessionOpManager<T>::createOp(Args&&... args) {
    auto s = session();
    TOp* op = m_pool->create(std::forward<Args>(args)..., s);
    registerNewOp(op);
    return op;
}

template<class T>
void SessionOpManager<T>::operator()(T* op) {
    doOp(op);
}

template<class T>
void SessionOpManager<T>::doOp(T* op) {
    m_prevOp = m_curOp;
    m_curOp = op;
    dispatchOp(op);
}

template<class T>
void SessionOpManager<T>::dispatchOp(Operation* op) {
    auto d = m_dispatcher;
    session()->template dispatch<true>([op, d] { d->pushOp(op); });
}

template<class T>
void SessionOpManager<T>::repeat() {
    doOp(m_curOp);
}

template class SessionOpManager<SessionReadOperation>;
template class SessionOpManager<SessionWriteOperation>;

template SessionWriteOperation* SessionOpManager<SessionWriteOperation>::createOp<const uint8_t*&, std::size_t&, bool>(const uint8_t*&, std::size_t&, bool&&);
template SessionReadOperation* SessionOpManager<SessionReadOperation>::createOp<uint8_t*&, std::size_t const&, bool const&>(uint8_t*&, std::size_t const&, bool const&);
