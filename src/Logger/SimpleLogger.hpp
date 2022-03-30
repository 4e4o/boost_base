#ifndef SIMPLE_LOGGER_HPP
#define SIMPLE_LOGGER_HPP

#include "ILogger.hpp"

class SimpleLogger : public ILogger {
public:
    SimpleLogger(bool autoFlush = true);
    ~SimpleLogger();

    void log(const std::string&) override;

private:
    const bool m_autoFlush;
};

#endif // SIMPLE_LOGGER_HPP
