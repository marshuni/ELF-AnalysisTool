#include "parsers/ElfHeaderParser.hpp"
#include "json/json.hpp"
#include <iomanip>
#include <sstream>

void ElfHeaderParser::parse(std::ifstream& ifs) {
    ifs.read(reinterpret_cast<char*>(&header), sizeof(header));
}

nlohmann::json ElfHeaderParser::toJSON() const {
    nlohmann::json j;

    std::string elfClass = (header.e_ident[EI_CLASS] == ELFCLASS64) ? "ELF64" :
                           (header.e_ident[EI_CLASS] == ELFCLASS32) ? "ELF32" : "Invalid";

    std::string dataEncoding = (header.e_ident[EI_DATA] == ELFDATA2LSB) ? "Little Endian" :
                                (header.e_ident[EI_DATA] == ELFDATA2MSB) ? "Big Endian" : "Unknown";

    std::ostringstream oss;
    oss << "0x" << std::hex << header.e_entry;

    j["elf_header"] = {
        { "class", elfClass },
        { "data_encoding", dataEncoding },
        { "entry_point", oss.str() },
        { "type", header.e_type },
        { "machine", header.e_machine },
        { "version", header.e_version },
        { "program_header_offset", header.e_phoff },
        { "section_header_offset", header.e_shoff },
        { "flags", header.e_flags },
        { "header_size", header.e_ehsize }
    };

    return j;
}
