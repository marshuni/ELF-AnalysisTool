#include "parsers/elf_header.h"
#include "elf/elf.h"
#include <string.h>

bool parse_elf_header(FILE* fp, cJSON* root) {
    if (!fp || !root) return false;

    Elf64_Ehdr header;

    // 回到文件开头
    fseek(fp, 0, SEEK_SET);

    // 读取 ELF Header
    if (fread(&header, 1, sizeof(header), fp) != sizeof(header)) {
        return false;
    }

    // 检查 ELF 魔数
    if (memcmp(header.e_ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "Not a valid ELF file.\n");
        return false;
    }

    cJSON* jheader = cJSON_CreateObject();

    const char* elf_class =
        (header.e_ident[EI_CLASS] == ELFCLASS64) ? "ELF64" :
        (header.e_ident[EI_CLASS] == ELFCLASS32) ? "ELF32" : "Invalid";

    const char* data_encoding =
        (header.e_ident[EI_DATA] == ELFDATA2LSB) ? "Little Endian" :
        (header.e_ident[EI_DATA] == ELFDATA2MSB) ? "Big Endian" : "Unknown";

    cJSON_AddStringToObject(jheader, "class", elf_class);
    cJSON_AddStringToObject(jheader, "data_encoding", data_encoding);
    cJSON_AddNumberToObject(jheader, "entry_point", header.e_entry);
    cJSON_AddNumberToObject(jheader, "type", header.e_type);
    cJSON_AddNumberToObject(jheader, "machine", header.e_machine);
    cJSON_AddNumberToObject(jheader, "version", header.e_version);
    cJSON_AddNumberToObject(jheader, "program_header_offset", header.e_phoff);
    cJSON_AddNumberToObject(jheader, "section_header_offset", header.e_shoff);
    cJSON_AddNumberToObject(jheader, "flags", header.e_flags);
    cJSON_AddNumberToObject(jheader, "header_size", header.e_ehsize);

    // 添加到总 JSON 结构
    cJSON_AddItemToObject(root, "elf_header", jheader);

    return true;
}
