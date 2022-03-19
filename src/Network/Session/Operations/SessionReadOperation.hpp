#ifndef SESSION_READ_OPERATION_H
#define SESSION_READ_OPERATION_H

#include "SessionOperation.hpp"
#include "Pools/Recycable.hpp"

class SessionReadOperation : public SessionOperation, public Recycable {
public:
    SessionReadOperation(uint8_t *ptr, std::size_t size, bool all, TSession);

    std::size_t readed() const;

private:
    void startImpl() override;

    uint8_t * const m_ptr;
    const std::size_t m_size;
    const bool m_all;
    std::size_t m_readed;
};

#endif // SESSION_READ_OPERATION_H
