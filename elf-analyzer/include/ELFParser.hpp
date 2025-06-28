#ifndef ELF_PARSER_HPP
#define ELF_PARSER_HPP

#include "IElfComponentParser.hpp"
#include <vector>
#include <memory>
#include <string>
#include "json/json.hpp"

class ElfParser {
private:
    std::string filePath;
    std::vector<std::unique_ptr<IElfComponentParser>> parsers;

public:
    explicit ElfParser(const std::string& path);

    void initParsers();  // 注册模块
    nlohmann::json parseAll();  // 返回合并 JSON
};

#endif