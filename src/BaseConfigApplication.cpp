#include "BaseConfigApplication.h"
#include "Config/Config.hpp"

#include <ThreadPool.hpp>
#include <Misc/Debug.hpp>

#include <boost/asio/post.hpp>

BaseConfigApplication::BaseConfigApplication(const std::string& n,
                                             int argc, char** argv)
    : AApplication(n, argc, argv) {
    debug_print(boost::format("BaseConfigApplication::BaseConfigApplication %1%") % this);
}

BaseConfigApplication::~BaseConfigApplication() {
    debug_print(boost::format("BaseConfigApplication::~BaseConfigApplication %1%") % this);
}

bool BaseConfigApplication::processArgs() {
    po::options_description desc("Options");
    desc.add_options()("c", po::value<std::string>(), "Config path");
    po::variables_map vm = parseCmdLine(desc);

    if (!vm.count("c")) {
        log("Must set config with --c <config_path>");
        return false;
    }

    m_configPath = vm["c"].as<std::string>();
    return true;
}

int BaseConfigApplication::exec() {
    if (!processArgs())
        return 1;

    m_threadPool.reset(createThreadPool());
    AApplication::exec();

    try {
        std::unique_ptr<Config> config(createConfig());
        Config::TItems items;

        if (!config->readConfig(items, m_configPath))
            return 2;

        start(items);

        m_threadPool->run();
        m_threadPool->stop(true);
    } catch (std::exception& e) {
        log(boost::format("BaseConfigApplication::run exception %1%") % e.what());
    }

    return 0;
}

ThreadPool* BaseConfigApplication::createThreadPool() {
    return new ThreadPool();
}

boost::asio::io_context &BaseConfigApplication::io() {
    return m_threadPool->io();
}

void BaseConfigApplication::onExitRequest() {
    // ensure that exit logic will run after m_threadPool is started
    boost::asio::post(m_threadPool->io(), [this]() {
        debug_print("BaseConfigApplication::onExitRequest");
        doExit();
        m_threadPool->stop(false);
    });
}
