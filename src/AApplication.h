#ifndef AAPPLICATION_H
#define AAPPLICATION_H

#include "Logger/ALogger.hpp"

#include <boost/program_options.hpp>

#define AAP             AApplication::app()
#define AAP_LOG(...)    AAP->logger()->log(__VA_ARGS__)

class AApplication {
public:
    AApplication(int argc, char** argv);
    virtual ~AApplication();

    ALogger* logger() const;

    virtual int exec();
    void quit();

    static AApplication* app() {
        return m_app;
    }

protected:
    boost::program_options::variables_map parseCmdLine(boost::program_options::options_description&);
    virtual void onExitRequest();

    // don't forget to call resetLogger when new logger is being detroyed!
    void setLogger(ALogger*);
    void resetLogger();

private:
    friend class AApPrivateAccessor;

    int m_argc;
    char** m_argv;
    ALogger* m_currentLogger;
    std::unique_ptr<ALogger> m_defaultLogger;

    static AApplication* m_app;
};

typedef std::unique_ptr<AApplication> TAApplication;

#endif // AAPPLICATION_H
