#ifndef SESSION_WRITER_HPP
#define SESSION_WRITER_HPP

#include "SessionOpManager.hpp"
#include "SessionWriteOperation.hpp"

#define BYTE(x) static_cast<uint8_t>(x)

// Адаптер для записи в сессию

class SessionWriter : public SessionOpManager<SessionWriteOperation> {
public:
    using SessionOpManager<SessionWriteOperation>::SessionOpManager;

    template<class CompletionFunc>
    void operator()(CompletionFunc&& e, std::initializer_list<uint8_t> l) {
        std::vector<uint8_t> data(l);
        operator()<true>(std::forward<CompletionFunc>(e), data.data(), data.size());
    }

    template<bool Copy = false, class CompletionFunc>
    void operator()(CompletionFunc&& e, const uint8_t *ptr, std::size_t size) {
        TOp *op = createOp(ptr, size, Copy);
        op->completed.connect([e = std::move(e), op, ptr](bool success) {
            if (success) {
                e();
            }
        });

        doOp(op);
    }
};

#endif // SESSION_WRITER_HPP
