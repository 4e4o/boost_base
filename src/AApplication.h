#ifndef AAPPLICATION_H
#define AAPPLICATION_H

#include "Logger/ILogger.hpp"

#include <boost/program_options.hpp>

#define AAP             AApplication::app()
#define AAP_LOG(...)    AAP->logger()->log(__VA_ARGS__)

class AApplication {
public:
    AApplication(const std::string&,
                 int argc, char** argv);
    ~AApplication();

    virtual ILogger* logger() const;

    virtual int exec();
    void quit();

    static AApplication* app() {
        return m_app;
    }

protected:
    boost::program_options::variables_map parseCmdLine(boost::program_options::options_description&);
    virtual void onExitRequest();

private:
    friend class AApPrivateAccessor;

    int m_argc;
    char** m_argv;
    std::string m_progName;
    std::unique_ptr<ILogger> m_logger;

    static AApplication* m_app;
};

#endif // AAPPLICATION_H
