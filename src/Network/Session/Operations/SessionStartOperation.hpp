#ifndef SESSION_START_OPERATION_H
#define SESSION_START_OPERATION_H

#include "SessionOperation.hpp"

class SessionStartOperation : public SessionOperation {
public:
    using SessionOperation::SessionOperation;

private:
    void startImpl() override;
};

#endif // SESSION_START_OPERATION_H
