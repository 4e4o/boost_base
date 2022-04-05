#ifndef ALOGGER_HPP
#define ALOGGER_HPP

#include <boost/format.hpp>
#include <string>

using fmt = boost::format;

class ALogger {
public:
    ALogger();
    virtual ~ALogger();

    void log(const std::string &msg);
    void log(const fmt& fmt);

protected:
    virtual void logImpl(const std::string&) = 0;

};

#endif // ALOGGER_HPP
