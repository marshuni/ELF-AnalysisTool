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

    // 魔数
    char magic_str[SELFMAG * 3 + 1] = {0};
    for (int i = 0; i < SELFMAG; ++i) {
        sprintf(magic_str + i * 3, "%02x ", header.e_ident[i]);
    }
    magic_str[SELFMAG * 3 - 1] = '\0';
    cJSON_AddStringToObject(jheader, "magic", magic_str);
    // ELF 类别
    cJSON_AddNumberToObject(jheader, "class", header.e_ident[EI_CLASS]);
    // 数据编码
    cJSON_AddNumberToObject(jheader, "data_encoding", header.e_ident[EI_DATA]);
    // 版本
    cJSON_AddNumberToObject(jheader, "version", header.e_ident[EI_VERSION]);
    // OS/ABI
    cJSON_AddNumberToObject(jheader, "os_abi", header.e_ident[EI_OSABI]);
    // ABI 版本
    cJSON_AddNumberToObject(jheader, "abi_version", header.e_ident[EI_ABIVERSION]);
    // 类型
    cJSON_AddNumberToObject(jheader, "type", header.e_type);
    // 机器类型
    cJSON_AddNumberToObject(jheader, "machine", header.e_machine);
    // 版本
    cJSON_AddNumberToObject(jheader, "version2", header.e_version);
    // 入口点
    cJSON_AddNumberToObject(jheader, "entry_point", header.e_entry);
    // 程序头表偏移
    cJSON_AddNumberToObject(jheader, "program_header_offset", header.e_phoff);
    // 节头表偏移
    cJSON_AddNumberToObject(jheader, "section_header_offset", header.e_shoff);
    // 标志
    cJSON_AddNumberToObject(jheader, "flags", header.e_flags);
    // ELF Header 大小
    cJSON_AddNumberToObject(jheader, "header_size", header.e_ehsize);
    // 程序头表项大小和数量
    cJSON_AddNumberToObject(jheader, "program_header_entry_size", header.e_phentsize);
    cJSON_AddNumberToObject(jheader, "program_header_num", header.e_phnum);
    // 节头表项大小和数量
    cJSON_AddNumberToObject(jheader, "section_header_entry_size", header.e_shentsize);
    cJSON_AddNumberToObject(jheader, "section_header_num", header.e_shnum);
    // 节头字符串表索引
    cJSON_AddNumberToObject(jheader, "section_header_string_index", header.e_shstrndx);
    // 添加到总 JSON 结构
    cJSON_AddItemToObject(root, "elf_header", jheader);

    return true;
}
