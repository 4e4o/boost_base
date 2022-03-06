#include "ConfigItem.hpp"

#include <Misc/AlwaysFalse.hpp>
#include <boost/json.hpp>

ConfigItem::~ConfigItem() {
}

bool ConfigItem::init(const boost::json::object&) {
    return true;
}

template<typename T, bool optional>
T ConfigItem::get(const boost::json::object& o, const std::string& key) {
    if (optional && !o.contains(key))
        return T();

    if constexpr (std::is_same_v<T, std::string>) {
        const boost::json::string &s1 = o.at(key).as_string();
        return std::string(s1.begin(), s1.end());
    } else if constexpr (std::is_same_v<T, std::int64_t>) {
        return o.at(key).as_int64();
    } else {
        static_assert (AlwaysFalse<T>::value, "Not implemented type");
    }
}

bool ConfigItem::isEnabled(const boost::json::object& o) {
    if (o.contains("enabled"))
        return o.at("enabled").as_bool();

    return true;
}

template std::int64_t ConfigItem::get<std::int64_t, false>(const boost::json::object&, const std::string&);
template std::string ConfigItem::get<std::string, false>(const boost::json::object&, const std::string&);

template std::int64_t ConfigItem::get<std::int64_t, true>(const boost::json::object&, const std::string&);
template std::string ConfigItem::get<std::string, true>(const boost::json::object&, const std::string&);
