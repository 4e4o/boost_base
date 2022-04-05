#include "BaseConfigApplication.h"
#include "Config/Config.hpp"

#include <ThreadPool.hpp>
#include <Logger/AsyncLogger.hpp>
#include <Misc/Debug.hpp>

using namespace boost::asio;
using namespace boost::program_options;

BaseConfigApplication::BaseConfigApplication(int argc, char** argv)
    : AApplication(argc, argv),
      m_config(new Config()) {
    debug_print_this("");
}

BaseConfigApplication::~BaseConfigApplication() {
    resetLogger();
    debug_print_this("");
}

bool BaseConfigApplication::processArgs() {
    options_description desc("Options");
    desc.add_options()("c", value<std::string>(), "Config path");
    variables_map vm = parseCmdLine(desc);

    if (!vm.count("c")) {
        AAP_LOG("Must set config with --c <config_path>");
        return false;
    }

    m_configPath = vm["c"].as<std::string>();
    return true;
}

Config* BaseConfigApplication::config() {
    return m_config.get();
}

int BaseConfigApplication::exec() {
    if (!processArgs())
        return 1;

    m_threadPool.reset(new ThreadPool());
    m_logger.reset(new AsyncLogger(io()));
    setLogger(m_logger.get());

    AApplication::exec();

    try {
        Config::TItems items;

        if (!m_config->readConfig(items, m_configPath))
            return 2;

        if (!start(items))
            return 1;

        m_logger->start();
        m_threadPool->run();
        m_threadPool->stop(true);
    } catch (const std::exception& e) {
        AAP_LOG(fmt("BaseConfigApplication exec exception %1%") % e.what());
    }

    return 0;
}

boost::asio::io_context &BaseConfigApplication::io() const {
    return m_threadPool->io();
}

void BaseConfigApplication::onExitRequest() {
    // ensure that exit logic will run after m_threadPool is started
    post([this] {
        debug_print_this("start");
        doExit();
        m_logger->stop();
        debug_print_this("before thread pool stop");
        m_threadPool->stop(false);
        debug_print_this("end");
    });
}
