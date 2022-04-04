#ifndef TYPE_NAME_HPP
#define TYPE_NAME_HPP

#include <string_view>

template <typename T>
consteval auto getTypeName() {
    const auto &str = __PRETTY_FUNCTION__;
    constexpr auto prettyStringSize = std::size(str);
#ifdef __clang__
    constexpr auto startPosOffset =
            __builtin_strlen("auto getTypeName() [T = ");
#else
    constexpr auto startPosOffset =
            __builtin_strlen("consteval auto getTypeName() [with T = ");
#endif
    constexpr auto endPosOffset = __builtin_strlen("] ");
    return std::string_view(&str[startPosOffset], &str[prettyStringSize - endPosOffset]);
}

#endif // TYPE_NAME_HPP
