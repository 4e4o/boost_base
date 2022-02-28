#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Config/ArrayParser.hpp"

class Config : public ArrayParser {
public:
    Config();
    virtual ~Config();

    void setKey(const std::string &key);
    bool readConfig(TItems&, const std::string&);

private:
    std::string m_key;
};

#endif /* CONFIG_HPP */
