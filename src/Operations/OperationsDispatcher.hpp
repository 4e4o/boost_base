#ifndef OPERATIONS_DISPATCHER_H
#define OPERATIONS_DISPATCHER_H

#include <list>

#include <boost/signals2/signal.hpp>

class Operation;

class OperationsDispatcher {
public:
    OperationsDispatcher();
    ~OperationsDispatcher();

    void start();
    void stop(bool force);

    void pushOp(Operation*);

    bool isFinished() const;
    bool isInactive() const;

    boost::signals2::signal<void()> finished;
    boost::signals2::signal<void()> inactive;
    boost::signals2::signal<void(Operation*)> opError;
    boost::signals2::signal<void(Operation*)> opAdded;

private:
    void processNext();
    void onOpResult(bool);
    void clear();
    void deleteOp(Operation*);
    void checkMoveToFinished();

    enum class State {
        INITIAL,
        RUNNING,
        STOPPED,    // новые операции не будут запущены, может иметь активные операции
        FINISHED,   // не имеет активных операций и новые не будут запущены
    };

    State m_state;
    Operation* m_currentOp;
    std::list<Operation*> m_operations;
};

#endif // OPERATIONS_DISPATCHER_H
