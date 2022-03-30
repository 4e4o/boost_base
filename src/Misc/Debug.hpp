#ifndef DEBUG_HPP
#define DEBUG_HPP

#include "AApplication.h"

#include <string_view>

#define METHOD_NAME             DebugUtils::methodName(__PRETTY_FUNCTION__)

#ifdef NDEBUG

#define debug_print(...)            { }
#define debug_print_func(...)       { }
#define debug_print_this(...)       { }

#define INIT_DEBUG_OBJECTS_COUNT    { }
#define DEBUG_OBJECTS_COUNT_INC     { }
#define DEBUG_OBJECTS_COUNT_DEC     { }

#else   // NDEBUG

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

#endif  // NDEBUG

class DebugUtils {
public:
    static constexpr std::string_view methodName(const char* prettyFunc) {
        const std::string_view prettyFunction(prettyFunc);
        const size_t end = prettyFunction.find("(");
        size_t begin = prettyFunction.rfind(" ", end);

        if (begin == std::string_view::npos)
            begin = 0;
        else
            begin++;

        return prettyFunction.substr(begin, end - begin);
    }
};

#endif /* DEBUG_HPP */
