#ifndef SESSION_READER_HPP
#define SESSION_READER_HPP

#include "SessionOpBase.hpp"
#include "Coroutine/Awaitables.hpp"

// Адаптер для чтения из сессии

class SessionReader : public SessionOpBase {
public:
    using SessionOpBase::SessionOpBase;

    TAwaitSize some(size_t maxSize = DEFAULT_BUFFER_SIZE);
    TAwaitSize all(size_t maxSize = DEFAULT_BUFFER_SIZE);
};

#endif // SESSION_READER_HPP
