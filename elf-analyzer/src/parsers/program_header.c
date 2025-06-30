#include "parsers/program_header.h"
#include "elf/elf.h"

bool parse_program_header(FILE* fp, cJSON* root) {
    if (!fp || !root) return false;

    // 保存当前位置，读取 e_phoff 等
    fseek(fp, 0, SEEK_SET);
    Elf64_Ehdr ehdr;
    if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) {
        return false;
    }

    if (ehdr.e_phoff == 0 || ehdr.e_phnum == 0) {
        return true; // 无 Program Header
    }

    fseek(fp, ehdr.e_phoff, SEEK_SET);

    cJSON* jph_table = cJSON_CreateArray();

    for (int i = 0; i < ehdr.e_phnum; ++i) {
        Elf64_Phdr phdr;
        if (fread(&phdr, 1, sizeof(phdr), fp) != sizeof(phdr)) {
            break;
        }

        cJSON* jentry = cJSON_CreateObject();
        cJSON_AddNumberToObject(jentry, "type", phdr.p_type);
        cJSON_AddNumberToObject(jentry, "offset", phdr.p_offset);
        cJSON_AddNumberToObject(jentry, "vaddr", phdr.p_vaddr);
        cJSON_AddNumberToObject(jentry, "paddr", phdr.p_paddr);
        cJSON_AddNumberToObject(jentry, "filesz", phdr.p_filesz);
        cJSON_AddNumberToObject(jentry, "memsz", phdr.p_memsz);
        cJSON_AddNumberToObject(jentry, "flags", phdr.p_flags);
        cJSON_AddNumberToObject(jentry, "align", phdr.p_align);

        cJSON_AddItemToArray(jph_table, jentry);
    }

    cJSON_AddItemToObject(root, "program_headers", jph_table);

    return true;
}
