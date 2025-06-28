#include "ElfParser.hpp"
#include "parsers/ElfHeaderParser.hpp"
// 后续模块可以继续 include 其他 parser

#include <fstream>
#include <iostream>

using namespace std;

ElfParser::ElfParser(const string& path) : filePath(path) {
    initParsers();
}

void ElfParser::initParsers() {
    parsers.emplace_back(make_unique<ElfHeaderParser>());
    // 后续添加其它模块...
}

nlohmann::json ElfParser::parseAll() {
    ifstream ifs(filePath, ios::binary);
    if (!ifs) {
        cerr << "Failed to open file: " << filePath << endl;
        return { {"error", "File open failed"} };
    }

    nlohmann::json result;

    for (const auto& parser : parsers) {
        // 注意：每次都从头读取
        ifs.clear();
        ifs.seekg(0);
        parser->parse(ifs);
        nlohmann::json j = parser->toJSON();
        result.update(j);
    }

    return result;
}