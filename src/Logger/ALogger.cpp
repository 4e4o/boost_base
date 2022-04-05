#include "ALogger.hpp"
#include "Misc/TimeUtils.hpp"

ALogger::ALogger() {
}

ALogger::~ALogger() {
}

void ALogger::log(const std::string &msg) {
    const std::string newMsg = "[" + TimeUtils::nowTimeOnly() + "] " + msg;
    logImpl(newMsg);
}

void ALogger::log(const fmt& fmt) {
    log(fmt.str());
}
