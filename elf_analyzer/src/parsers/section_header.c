#include "parsers/section_header.h"
#include "elf/elf.h"
#include <stdlib.h>
#include <string.h>

static const char* get_section_type(uint32_t type) {
    switch (type) {
        case SHT_NULL: return "NULL";
        case SHT_PROGBITS: return "PROGBITS";
        case SHT_SYMTAB: return "SYMTAB";
        case SHT_STRTAB: return "STRTAB";
        case SHT_RELA: return "RELA";
        case SHT_HASH: return "HASH";
        case SHT_DYNAMIC: return "DYNAMIC";
        case SHT_NOTE: return "NOTE";
        case SHT_NOBITS: return "NOBITS";
        case SHT_REL: return "REL";
        case SHT_SHLIB: return "SHLIB";
        case SHT_DYNSYM: return "DYNSYM";
        default: return "UNKNOWN";
    }
}

bool parse_section_headers(FILE* fp, cJSON* root) {
    if (!fp || !root) return false;

    // 读取 ELF Header
    fseek(fp, 0, SEEK_SET);
    Elf64_Ehdr ehdr;
    if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) return false;

    if (ehdr.e_shoff == 0 || ehdr.e_shnum == 0) return true; // 无节头表

    // 读取所有节头
    Elf64_Shdr* sh_table = malloc(ehdr.e_shentsize * ehdr.e_shnum);
    if (!sh_table) return false;

    fseek(fp, ehdr.e_shoff, SEEK_SET);
    if (fread(sh_table, ehdr.e_shentsize, ehdr.e_shnum, fp) != ehdr.e_shnum) {
        free(sh_table);
        return false;
    }

    // 读取节名字符串表
    const Elf64_Shdr* shstr_shdr = &sh_table[ehdr.e_shstrndx];
    char* shstrtab = malloc(shstr_shdr->sh_size);
    if (!shstrtab) {
        free(sh_table);
        return false;
    }

    fseek(fp, shstr_shdr->sh_offset, SEEK_SET);
    if (fread(shstrtab, 1, shstr_shdr->sh_size, fp) != shstr_shdr->sh_size) {
        free(shstrtab);
        free(sh_table);
        return false;
    }

    cJSON* jsections = cJSON_CreateArray();

    for (int i = 0; i < ehdr.e_shnum; ++i) {
        const Elf64_Shdr* sh = &sh_table[i];
        const char* name = (sh->sh_name < shstr_shdr->sh_size) ? (shstrtab + sh->sh_name) : "<invalid>";

        cJSON* jsec = cJSON_CreateObject();
        cJSON_AddNumberToObject(jsec, "index", i);
        cJSON_AddStringToObject(jsec, "name", name);
        cJSON_AddStringToObject(jsec, "type", get_section_type(sh->sh_type));
        cJSON_AddNumberToObject(jsec, "addr", sh->sh_addr);
        cJSON_AddNumberToObject(jsec, "offset", sh->sh_offset);
        cJSON_AddNumberToObject(jsec, "size", sh->sh_size);
        cJSON_AddNumberToObject(jsec, "flags", sh->sh_flags);

        cJSON_AddItemToArray(jsections, jsec);
    }

    cJSON_AddItemToObject(root, "section_headers", jsections);

    free(shstrtab);
    free(sh_table);
    return true;
}
