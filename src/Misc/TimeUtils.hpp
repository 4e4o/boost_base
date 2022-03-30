#ifndef TIME_UTILS_HPP
#define TIME_UTILS_HPP

#include <string>

class TimeUtils {
public:
    static std::string nowTimeOnly(bool withMsec = true);
};

#endif /* TIME_UTILS_HPP */
