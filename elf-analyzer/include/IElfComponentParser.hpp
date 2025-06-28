#ifndef I_ELF_COMPONENT_PARSER_HPP
#define I_ELF_COMPONENT_PARSER_HPP

#include <fstream>
#include "json/json.hpp"

class IElfComponentParser {
public:
    virtual void parse(std::ifstream& ifs) = 0;
    virtual nlohmann::json toJSON() const = 0;
    virtual ~IElfComponentParser() = default;
};

#endif
