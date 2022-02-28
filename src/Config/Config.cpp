#include "Config.hpp"

#include <AApplication.h>

#include <iostream>
#include <fstream>

#include <boost/json.hpp>

#define DEFAULT_KEY "config"

Config::Config()
    : m_key(DEFAULT_KEY) {
}

Config::~Config() {
}

bool Config::readConfig(TItems& result, const std::string& config_file) {
    try {
        std::ifstream ifs(config_file);
        std::string input(std::istreambuf_iterator<char>(ifs), {});

        auto parsed_data = boost::json::parse(input);
        const auto& items = parsed_data.at(m_key);

        return parseArray(items.as_array(), result);
    } catch (std::exception const& e) {
        AAP->log(boost::format("Config parse error: %1%") % e.what());
        return false;
    }

    return true;
}

void Config::setKey(const std::string &key) {
    m_key = key;
}
