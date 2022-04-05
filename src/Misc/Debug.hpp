#ifndef DEBUG_HPP
#define DEBUG_HPP

#ifdef NDEBUG

#define debug_print(...)
#define debug_print_func(...)
#define debug_print_this(...)

#define INIT_DEBUG_OBJECTS_COUNT(...)
#define DEBUG_OBJECTS_COUNT_INC(...)
#define DEBUG_OBJECTS_COUNT_DEC(...)

#else   // NDEBUG

#include "AApplication.h"

#include <string_view>

#define METHOD_NAME             DebugUtils::methodName(__PRETTY_FUNCTION__)

#define debug_print(...)        { AAP_LOG(__VA_ARGS__); }
#define debug_print_func(...)   { debug_print(fmt("%1% %2%") % METHOD_NAME % (__VA_ARGS__)); }
#define debug_print_this(...)   { debug_print_func(fmt("%1% %2%") % this % (__VA_ARGS__)); }

#define DEBUG_OBJECTS_COUNTER_NAME(obj) g_debug_##obj##_count

#define INIT_DEBUG_OBJECTS_COUNT(obj_name)  static std::atomic<int> DEBUG_OBJECTS_COUNTER_NAME(obj_name)(0);
#define DEBUG_OBJECTS_COUNT_INC(obj_name)  {                                    \
    DEBUG_OBJECTS_COUNTER_NAME(obj_name)++;                                     \
    debug_print_this(fmt("count %1%") % DEBUG_OBJECTS_COUNTER_NAME(obj_name));  \
    }

#define DEBUG_OBJECTS_COUNT_DEC(obj_name)  {                                    \
    DEBUG_OBJECTS_COUNTER_NAME(obj_name)--;                                     \
    debug_print_this(fmt("count %1%") % DEBUG_OBJECTS_COUNTER_NAME(obj_name));  \
    }

class DebugUtils {
public:
    // innaccurate, for debug purposes only!
    static consteval std::string_view methodName(const char* prettyFunc) {
        using str = std::string_view;
        const str signature(prettyFunc);

        auto tokenizer = [](const str& v, char splitter, auto&& visitor) {
            const str groups("()<>[]");
            size_t tokenBegin = 0;
            int depth = 0;

            for (size_t i = 0; i < v.length() ; i++) {
                if ((v[i] == splitter) && (depth == 0)) {
                    const std::string_view token = v.substr(tokenBegin, i - tokenBegin);
                    if (!visitor(token))
                        return;

                    tokenBegin = i + 1;
                }

                for (size_t j = 0 ; j < groups.length() ; j += 2) {
                    if (v[i] == groups[j]) {
                        depth++;
                    } else if (v[i] == groups[j + 1]) {
                        depth--;
                    }
                }
            }

            if (tokenBegin < v.length())
                visitor(v.substr(tokenBegin));
        };

        std::string_view classMethod = signature;

        // try to find class::method word
        tokenizer(signature, ' ', [&classMethod](const std::string_view& v) {
            // skip [....
            if (*v.begin() != '[')
                classMethod = v;

            return true;
        });

        std::string_view withoutArgs = classMethod;

        // trim arguments part
        tokenizer(classMethod, '(', [&withoutArgs](const std::string_view& v) {
            withoutArgs = v;
            return false;
        });

        return withoutArgs;
    }
};

#endif  // NDEBUG

#endif /* DEBUG_HPP */
