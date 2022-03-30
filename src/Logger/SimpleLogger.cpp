#include "SimpleLogger.hpp"

#include <syslog.h>

//#define LOG_TO_SYSLOG   1
#define LOG_TO_CONSOLE  1

SimpleLogger::SimpleLogger(bool autoFlush)
    : m_autoFlush(autoFlush) {
}

SimpleLogger::~SimpleLogger() {
}

void SimpleLogger::log(const std::string &msg) {
#ifdef LOG_TO_CONSOLE
    printf("%s\n", msg.c_str());
    if (m_autoFlush) {
        fflush(stdout);
    }
#endif // LOG_TO_CONSOLE

#ifdef LOG_TO_SYSLOG
    syslog(LOG_NOTICE, "%s", msg.c_str());
#endif // LOG_TO_SYSLOG
}
