#ifndef SESSION_WRITE_OPERATION_H
#define SESSION_WRITE_OPERATION_H

#include "SessionOperation.hpp"
#include "Pools/Recycable.hpp"

class SessionWriteOperation : public SessionOperation, public Recycable {
public:
    SessionWriteOperation(const uint8_t *ptr, std::size_t size, bool copy, TSession);
    ~SessionWriteOperation();

private:
    void startImpl() override;

    const uint8_t * const m_ptr;
    const std::size_t m_size;
    const bool m_copy;
};

#endif // SESSION_WRITE_OPERATION_H
