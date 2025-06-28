#ifndef ELF_HEADER_PARSER_HPP
#define ELF_HEADER_PARSER_HPP

#include "IElfComponentParser.hpp"
#include "elf/elf.h"

class ElfHeaderParser : public IElfComponentParser {
private:
    Elf64_Ehdr header;

public:
    void parse(std::ifstream& ifs) override;
    nlohmann::json toJSON() const override;
};

#endif