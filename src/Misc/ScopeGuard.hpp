#ifndef SCOPE_GUARD_H
#define SCOPE_GUARD_H

#include <functional>

class ScopeGuard {
public:
    template<class Callable>
    ScopeGuard(Callable&& c)
        : m_func(std::forward<Callable>(c)) { }

    ScopeGuard(ScopeGuard && other)
        : m_func(std::move(other.m_func)) {
        other.m_func = nullptr;
    }

    ScopeGuard(const ScopeGuard&) = delete;

    ~ScopeGuard() {
        if(m_func) m_func(); // must not throw
    }

    void dismiss() noexcept {
        m_func = nullptr;
    }

    void operator = (const ScopeGuard&) = delete;

private:
    std::function<void()> m_func;
};

#endif // SCOPE_GUARD_H
