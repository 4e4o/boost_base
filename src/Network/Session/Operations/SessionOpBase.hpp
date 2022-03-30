#ifndef SESSION_OP_BASE_HPP
#define SESSION_OP_BASE_HPP

#include <vector>
#include <stdint.h>

#define AS_BYTE(x) (static_cast<uint8_t>(x))

class Session;

class SessionOpBase {
public:
    SessionOpBase(Session*);
    virtual ~SessionOpBase();

    const uint8_t* ptr() const;
    uint8_t* ptr();

    void setBufferSize(const std::size_t&);

protected:
    static constexpr int DEFAULT_BUFFER_SIZE = 64 * 2 * 1024;

    typedef std::vector<uint8_t> TBuffer;

    TBuffer& buffer();
    Session* session() const;

private:
    Session* m_session;
    TBuffer m_buffer;
};

#endif // SESSION_OP_BASE_HPP
