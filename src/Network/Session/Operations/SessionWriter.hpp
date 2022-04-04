#ifndef SESSION_WRITER_HPP
#define SESSION_WRITER_HPP

#include "SessionOpBase.hpp"
#include "Coroutine/Awaitables.hpp"

// Адаптер для чтения из сессии

class Session;

// TODO remove it when bug will be fixed:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=103871
#define CO_SES_WRITE(s, ...) { \
    std::vector<uint8_t> buf({__VA_ARGS__}); \
    co_await (s)->writer().all(std::move(buf)); \
}

class SessionWriter : public SessionOpBase {
public:
    using SessionOpBase::SessionOpBase;

    //     TAwaitVoid all(std::initializer_list<uint8_t> l)
    template<class Container = TBuffer>
    TAwaitSize all(Container&& c) {
        buffer() = std::move(c);
        return all(ptr(), buffer().size());
    }

    TAwaitSize all(const uint8_t*, size_t size);
};

#endif // SESSION_WRITER_HPP
