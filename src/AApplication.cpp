#include "AApplication.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <stdarg.h>

//#define LOG_TO_SYSLOG   1
#define LOG_TO_CONSOLE  1

AApplication* AApplication::m_app = nullptr;

class AApPrivateAccessor {
public:
    static void callOnExit() {
        if (g_oneExit.test_and_set())
            return;

        AAP->onExitRequest();
    }

    static std::atomic_flag g_oneExit;
};

std::atomic_flag AApPrivateAccessor::g_oneExit = ATOMIC_FLAG_INIT;

static void exitSignalHandler(int) {
    AApPrivateAccessor::callOnExit();
}

static void setSignalHandler(int sig, __sighandler_t h) {
    struct sigaction action{};
    action.sa_handler = h;
    sigemptyset(&action.sa_mask);
    sigaction(sig, &action, NULL);
}

AApplication::AApplication(const std::string& name, int argc, char** argv)
    : m_progName(name), m_argc(argc), m_argv(argv) {
    m_app = this;
}

AApplication::~AApplication() {
}

int AApplication::AApplication::exec() {
    setSignalHandler(SIGINT, exitSignalHandler);
    setSignalHandler(SIGTERM, exitSignalHandler);
    return 0;
}

void AApplication::quit() {
    AApPrivateAccessor::callOnExit();
}

po::variables_map AApplication::parseCmdLine(po::options_description& desc) {
    try {
        po::variables_map vm;
        po::parsed_options parsed = po::command_line_parser(m_argc, m_argv).options(desc).run();
        po::store(parsed, vm);
        po::notify(vm);
        return vm;
    } catch(std::exception& ex) {
        log("Error parsing command line arguments");
    }

    return po::variables_map();
}

void AApplication::consoleLog(const std::string &msg) {
    printf("%s\n", msg.c_str());
    fflush(stdout);
}

void AApplication::log(const boost::format& fmt) {
    log(fmt.str());
}

void AApplication::log(const std::string &msg) {
#ifdef LOG_TO_CONSOLE
    consoleLog(msg);
#endif // LOG_TO_CONSOLE

#ifdef LOG_TO_SYSLOG
    syslog(LOG_NOTICE, "%s", msg.c_str());
#endif // LOG_TO_SYSLOG
}
