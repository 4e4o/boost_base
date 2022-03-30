#ifndef ILOGGER_HPP
#define ILOGGER_HPP

#include <boost/format.hpp>
#include <string>

using fmt = boost::format;

class ILogger {
public:
    ILogger() { }
    virtual ~ILogger() { }

    virtual void log(const std::string&) = 0;

    void log(const fmt& fmt) {
        log(fmt.str());
    }
};

#endif // ILOGGER_HPP
