#include "ArrayParser.hpp"

#include <AApplication.h>

#include <iostream>
#include <fstream>

#include <boost/json/src.hpp>

ArrayParser::ArrayParser() {
}

ArrayParser::~ArrayParser() {
}

bool ArrayParser::parseArray(const boost::json::array& input, TItems& result) {
    try {
        for (const auto& item : input) {
            const boost::json::object& obj = item.as_object();

            if (!ConfigItem::isEnabled(obj))
                continue;

            std::unique_ptr<ConfigItem> i(create<ConfigItem>(obj));

            if (i.get() == nullptr)
                continue;

            if (!i->init(obj))
                return false;

            result.emplace_back(i.release());
        }
    } catch (std::exception const& e) {
        AAP_LOG(fmt("Config array parse error: %1%") % e.what());
        return false;
    }

    return true;
}
