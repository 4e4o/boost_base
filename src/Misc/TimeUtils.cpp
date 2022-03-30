#include "TimeUtils.hpp"

#include <chrono>
#include <sstream>
#include <iomanip>

using namespace std::chrono;

std::string TimeUtils::nowTimeOnly(bool withMsec) {
    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    auto timer = system_clock::to_time_t(now);
    std::tm bt = *std::localtime(&timer);
    std::ostringstream oss;
    oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS

    if (withMsec) {
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    }

    return oss.str();
}
