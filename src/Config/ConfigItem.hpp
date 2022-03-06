#ifndef CONFIG_ITEM_HPP
#define CONFIG_ITEM_HPP

#include <string>

namespace boost::json {
class string;
class object;
class value;
}

class ConfigItem {
public:
    virtual ~ConfigItem();

    virtual bool init(const boost::json::object&);

    template<typename T, bool optional = false>
    static T get(const boost::json::object&, const std::string&);
    static bool isEnabled(const boost::json::object&);
};

#endif /* CONFIG_ITEM_HPP */
