#include "parsers/ElfHeaderParser.hpp"
#include "json/json.hpp"
#include <iomanip>
#include <sstream>

using namespace std;

void ElfHeaderParser::parse(ifstream& ifs) {
    ifs.read(reinterpret_cast<char*>(&header), sizeof(header));
}

nlohmann::json ElfHeaderParser::toJSON() const {
    nlohmann::json j;

    // Magic bytes
    std::ostringstream magic_oss;
    magic_oss << std::hex << std::setfill('0');
    for (int i = 0; i < EI_NIDENT; ++i) {
        magic_oss << std::setw(2) << static_cast<int>(header.e_ident[i]);
        if (i != EI_NIDENT - 1) magic_oss << " ";
    }

    // ELF Class
    std::string elfClass = (header.e_ident[EI_CLASS] == ELFCLASS64) ? "ELF64" :
                           (header.e_ident[EI_CLASS] == ELFCLASS32) ? "ELF32" : "Invalid";

    // Data Encoding
    std::string dataEncoding = (header.e_ident[EI_DATA] == ELFDATA2LSB) ? "Little Endian" :
                               (header.e_ident[EI_DATA] == ELFDATA2MSB) ? "Big Endian" : "Unknown";

    // OS/ABI
    std::string osabi;
    switch (header.e_ident[EI_OSABI]) {
        case ELFOSABI_SYSV: osabi = "UNIX - System V"; break;
        case ELFOSABI_HPUX: osabi = "HP-UX"; break;
        case ELFOSABI_FREEBSD: osabi = "FreeBSD"; break;
        case ELFOSABI_ARM: osabi = "ARM"; break;
        case ELFOSABI_STANDALONE: osabi = "Standalone"; break;
        default: osabi = "Unknown"; break;
    }

    // ABI Version
    uint8_t abi_version = header.e_ident[EI_ABIVERSION];

    // Entry point
    std::ostringstream entry_oss;
    entry_oss << "0x" << std::hex << header.e_entry;

    j["elf_header"] = {
        { "magic", magic_oss.str() },
        { "class", elfClass },
        { "data_encoding", dataEncoding },
        { "os_abi", osabi },
        { "abi_version", abi_version },
        { "type", header.e_type },
        { "machine", header.e_machine },
        { "version", header.e_version },
        { "entry_point", entry_oss.str() },
        { "program_header_offset", header.e_phoff },
        { "section_header_offset", header.e_shoff },
        { "flags", header.e_flags },
        { "header_size", header.e_ehsize },
        { "program_header_entry_size", header.e_phentsize },
        { "program_header_num", header.e_phnum },
        { "section_header_entry_size", header.e_shentsize },
        { "section_header_num", header.e_shnum },
        { "section_header_string_index", header.e_shstrndx }
    };

    return j;
}
