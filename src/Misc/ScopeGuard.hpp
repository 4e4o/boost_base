#ifndef SCOPE_GUARD_H
#define SCOPE_GUARD_H

#include <functional>

class ScopeGuard {
public:
    template<class Callable>
    ScopeGuard(Callable && undo_func)
        : f(std::forward<Callable>(undo_func)) { }

    ScopeGuard(ScopeGuard && other) : f(std::move(other.f)) {
        other.f = nullptr;
    }

    ScopeGuard(const ScopeGuard&) = delete;

    ~ScopeGuard() {
        if(f) f(); // must not throw
    }

    void dismiss() noexcept {
        f = nullptr;
    }

    void operator = (const ScopeGuard&) = delete;

private:
    std::function<void()> f;
};

#endif // SCOPE_GUARD_H
