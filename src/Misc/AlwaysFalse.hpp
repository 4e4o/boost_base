#ifndef ALWAYS_FALSE_HPP
#define ALWAYS_FALSE_HPP

template <typename... Ts>
struct AlwaysFalse {
    static constexpr bool value = false;
};

#endif // ALWAYS_FALSE_HPP
