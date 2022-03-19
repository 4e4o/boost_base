#ifndef SESSION_OP_MANAGER_HPP
#define SESSION_OP_MANAGER_HPP

#include "Misc/LambdaTraits.hpp"

#include <memory>
#include <functional>

class Session;
class Operation;
class OperationsDispatcher;
template<typename T> class TSObjPool1;

template<class T>
class SessionOpManager {
public:
    using TOp = T;
    typedef std::function<std::shared_ptr<Session>()> TSessionGetter;

    SessionOpManager(const TSessionGetter&, OperationsDispatcher* d);
    virtual ~SessionOpManager();

    void operator()(T* op);
    void repeat();

protected:
    void doOp(T* op);

    template<typename... Args>
    TOp* createOp(Args&&... args);

private:
    typedef TSObjPool1<T> TPool;

    void dispatchOp(Operation* op);
    void registerNewOp(T* op);
    std::shared_ptr<Session> session();

    T* m_curOp;
    T* m_prevOp;
    T* m_toRecycle;
    bool m_sessionReceived;
    TSessionGetter m_sessionInit;
    std::unique_ptr<TPool> m_pool;
    std::weak_ptr<Session> m_session;
    OperationsDispatcher *m_dispatcher;    
};

#endif // SESSION_OP_MANAGER_HPP
