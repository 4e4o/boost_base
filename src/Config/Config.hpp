#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Misc/GenericFactory.hpp"
#include "ConfigItem.hpp"

#include <list>
#include <memory>

namespace boost::json {
class object;
}

class Config : public GenericFactory<ConfigItem> {
public:
    typedef std::list<std::unique_ptr<ConfigItem>> TItems;

    Config(const std::string&);
    virtual ~Config();

    bool readConfig(TItems&, const std::string&);

protected:
    virtual ConfigItem* parseItem(const boost::json::object&) = 0;

private:
    std::string m_key;
};

#endif /* CONFIG_HPP */
