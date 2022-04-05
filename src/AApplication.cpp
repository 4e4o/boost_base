#include "AApplication.h"
#include "Logger/SimpleLogger.hpp"

#include <signal.h>

using namespace boost::program_options;

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

AApplication::AApplication(int argc, char** argv)
    : m_argc(argc), m_argv(argv),
      m_defaultLogger(new SimpleLogger()) {
    m_app = this;
    resetLogger();
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

void AApplication::onExitRequest() {
}

variables_map AApplication::parseCmdLine(options_description& desc) {
    try {
        variables_map vm;
        parsed_options parsed = command_line_parser(m_argc, m_argv).options(desc).run();
        store(parsed, vm);
        notify(vm);
        return vm;
    } catch(std::exception& ex) {
        AAP_LOG(fmt("Error parsing command line arguments %1%") % ex.what());
    }

    return variables_map();
}

void AApplication::setLogger(ALogger* l) {
    m_currentLogger = l;
}

void AApplication::resetLogger() {
    m_currentLogger = m_defaultLogger.get();
}

ALogger* AApplication::logger() const {
    return m_currentLogger;
}
