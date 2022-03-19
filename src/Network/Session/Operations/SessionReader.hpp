#ifndef SESSION_READER_HPP
#define SESSION_READER_HPP

#include "SessionOpManager.hpp"
#include "SessionReadOperation.hpp"

#include <array>

// Адаптер для чтения из сессии

class SessionReader : public SessionOpManager<SessionReadOperation> {
public:
    using SessionOpManager<SessionReadOperation>::SessionOpManager;

    template<class CompletionFunc>
    void operator()(CompletionFunc&& e, size_t maxSize = READ_BUFFER_SIZE) {
        operator()(std::forward<CompletionFunc>(e), m_readBuffer, maxSize);
    }

    template<class CompletionFunc, typename Container>
    void operator()(CompletionFunc&& e, Container& container, size_t maxSize = READ_BUFFER_SIZE) {
        typedef LambdaTraits<CompletionFunc> Traits;
        typedef typename Traits::args_tuple TArgs;

        static constexpr bool PARTIAL = std::is_same<TArgs, std::tuple<const uint8_t*, std::size_t>>::value;
        static constexpr bool FULL = std::is_same<TArgs, std::tuple<const uint8_t*>>::value;

        static_assert(FULL || PARTIAL, "use (const uint8_t *ptr, std::size_t size) or (const uint8_t *ptr) lambda");

        const std::size_t size = std::min(maxSize, container.size());
        uint8_t *ptr = container.data();
        TOp *op = createOp(ptr, size, FULL);

        op->completed.connect([e = std::move(e), op, ptr](bool success) {
            if (success) {
                if constexpr (PARTIAL) {
                    e(ptr, op->readed());
                } else {
                    e(ptr);
                }
            }
        });

        doOp(op);
    }

private:
    static constexpr int READ_BUFFER_SIZE = 64 * 2 * 1024;

    std::array<uint8_t, READ_BUFFER_SIZE> m_readBuffer;
};

#endif // SESSION_READER_HPP
