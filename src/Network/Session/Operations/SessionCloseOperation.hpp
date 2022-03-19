#ifndef SESSION_CLOSE_OPERATION_H
#define SESSION_CLOSE_OPERATION_H

#include "SessionOperation.hpp"

class SessionCloseOperation : public SessionOperation {
public:
    using SessionOperation::SessionOperation;

private:
    void startImpl() override;
};

#endif // SESSION_CLOSE_OPERATION_H
