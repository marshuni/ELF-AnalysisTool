#include "parsers/relocation_table.h"
#include "elf/elf.h"
#include <stdlib.h>

bool parse_relocation_tables(FILE* fp, cJSON* root) {
    fseek(fp, 0, SEEK_SET);
    Elf64_Ehdr ehdr;
    if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) return false;

    fseek(fp, ehdr.e_shoff, SEEK_SET);
    Elf64_Shdr* sh_table = (Elf64_Shdr*)malloc(ehdr.e_shentsize * ehdr.e_shnum);
    if (!sh_table) return false;
    fread(sh_table, ehdr.e_shentsize, ehdr.e_shnum, fp);

    cJSON* jrelocs = cJSON_CreateArray();

    for (int i = 0; i < ehdr.e_shnum; ++i) {
        const Elf64_Shdr* sh = &sh_table[i];
        if (sh->sh_type != SHT_RELA) continue;

        // 为当前重定位节创建JSON对象
        cJSON* jrela_section = cJSON_CreateObject();

        // 获取节名字符串表
        const Elf64_Shdr* shstr_shdr = &sh_table[ehdr.e_shstrndx];
        char* shstrtab = (char*)malloc(shstr_shdr->sh_size);
        fseek(fp, shstr_shdr->sh_offset, SEEK_SET);
        fread(shstrtab, 1, shstr_shdr->sh_size, fp);

        // 添加节名
        cJSON_AddStringToObject(jrela_section, "name", (sh->sh_name < shstr_shdr->sh_size) ? (shstrtab + sh->sh_name) : "<invalid>");

        // 创建当前节的重定位项数组
        cJSON* jentries = cJSON_CreateArray();

        Elf64_Rela* rela = (Elf64_Rela*)malloc(sh->sh_size);
        if (!rela) {
            free(shstrtab);
            continue;
        }
        fseek(fp, sh->sh_offset, SEEK_SET);
        fread(rela, 1, sh->sh_size, fp);

        int rela_count = sh->sh_size / sizeof(Elf64_Rela);
        for (int j = 0; j < rela_count; ++j) {
            Elf64_Rela* r = &rela[j];
            cJSON* jentry = cJSON_CreateObject();
            cJSON_AddNumberToObject(jentry, "offset", r->r_offset);
            cJSON_AddNumberToObject(jentry, "info", r->r_info);
            cJSON_AddNumberToObject(jentry, "addend", r->r_addend);
            cJSON_AddItemToArray(jentries, jentry);
        }
        cJSON_AddItemToObject(jrela_section, "entries", jentries);
        cJSON_AddItemToArray(jrelocs, jrela_section);

        free(rela);
        free(shstrtab);
    }

    cJSON_AddItemToObject(root, "relocations", jrelocs);
    free(sh_table);
    return true;
}
