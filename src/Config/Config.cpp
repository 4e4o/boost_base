#include "Config.hpp"

#include <AApplication.h>

#include <iostream>
#include <fstream>

#include <boost/json.hpp>

Config::Config(const std::string& key)
    : m_key(key) {
}

Config::~Config() {
}

bool Config::readConfig(TItems& result, const std::string& config_file) {
    try {
        std::ifstream ifs(config_file);
        std::string input(std::istreambuf_iterator<char>(ifs), {});

        auto parsed_data = boost::json::parse(input);
        const auto& items = parsed_data.at(m_key);

        for (const auto& item : items.as_array()) {
            const boost::json::object& obj = item.as_object();

            if (!ConfigItem::isEnabled(obj))
                continue;

            ConfigItem *i = parseItem(obj);

            if (i == nullptr)
                continue;

            result.emplace_back(i);
        }
    } catch (std::exception const& e) {
        AAP->log(boost::format("Config parse error: %1%") % e.what());
        return false;
    }

    return true;
}
