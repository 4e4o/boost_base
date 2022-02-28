#ifndef AAPPLICATION_H
#define AAPPLICATION_H

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include <string>

#define AAP AApplication::app()

namespace po = boost::program_options;

class AApplication {
public:
    AApplication(const std::string&,
                 int argc, char** argv);
    ~AApplication();

    void log(const std::string&);
    void log(const boost::format&);

    static AApplication* app() {
        return m_app;
    }

    virtual int exec();
    void quit();

protected:    
    po::variables_map parseCmdLine(po::options_description&);
    virtual void onExitRequest() { }
    virtual void consoleLog(const std::string&);

private:
    friend class AApPrivateAccessor;

    std::string m_progName;
    int m_argc;
    char** m_argv;

    static AApplication* m_app;
};

#endif // AAPPLICATION_H
