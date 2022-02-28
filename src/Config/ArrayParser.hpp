#ifndef ARRAY_PARSER_HPP
#define ARRAY_PARSER_HPP

#include "Misc/GenericFactory.hpp"
#include "ConfigItem.hpp"

#include <list>
#include <memory>

namespace boost::json {
class array;
}

class ArrayParser : public GenericFactory<ConfigItem> {
public:
    typedef std::list<std::unique_ptr<ConfigItem>> TItems;

    ArrayParser();
    virtual ~ArrayParser();

    bool parseArray(const boost::json::array&, TItems&);
};

#endif /* ARRAY_PARSER_HPP */
