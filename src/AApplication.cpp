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

AApplication::AApplication(const std::string& name, int argc, char** argv)
    : m_argc(argc), m_argv(argv),
      m_progName(name), m_logger(new SimpleLogger()) {
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
        AAP_LOG("Error parsing command line arguments");
    }

    return variables_map();
}

ILogger* AApplication::logger() const {
    return m_logger.get();
}
