#ifndef SIMPLE_LOGGER_HPP
#define SIMPLE_LOGGER_HPP

#include "ALogger.hpp"

class SimpleLogger : public ALogger {
public:
    SimpleLogger(bool autoFlush = true);
    ~SimpleLogger();

protected:
    void logImpl(const std::string&) override;

private:
    const bool m_autoFlush;
};

#endif // SIMPLE_LOGGER_HPP
