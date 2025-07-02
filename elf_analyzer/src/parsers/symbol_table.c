#include "parsers/symbol_table.h"
#include "elf/elf.h"
#include <stdlib.h>
#include <string.h>

bool parse_symbol_table(FILE* fp, cJSON* root) {
    // 读取ELF头
    fseek(fp, 0, SEEK_SET);
    Elf64_Ehdr ehdr;
    if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) return false;

    // 读取节头表到内存
    fseek(fp, ehdr.e_shoff, SEEK_SET);
    Elf64_Shdr* sh_table = malloc(ehdr.e_shentsize * ehdr.e_shnum);
    if (!sh_table) return false;
    fread(sh_table, ehdr.e_shentsize, ehdr.e_shnum, fp);

    // 读取节名字符串表到内存
    const Elf64_Shdr* shstr_shdr = &sh_table[ehdr.e_shstrndx];
    char* shstrtab = malloc(shstr_shdr->sh_size);
    fseek(fp, shstr_shdr->sh_offset, SEEK_SET);
    fread(shstrtab, 1, shstr_shdr->sh_size, fp);

    cJSON* jsymbols = cJSON_CreateArray();

    for (int i = 0; i < ehdr.e_shnum; ++i) {
        // 遍历节头表中的每个节 只处理符号表节
        const Elf64_Shdr* sh = &sh_table[i];
        if (sh->sh_type != SHT_SYMTAB && sh->sh_type != SHT_DYNSYM) continue;

        // 读取符号表到内存
        Elf64_Sym* symtab = malloc(sh->sh_size);
        if (!symtab) continue;
        fseek(fp, sh->sh_offset, SEEK_SET);
        fread(symtab, 1, sh->sh_size, fp);
        int sym_count = sh->sh_size / sizeof(Elf64_Sym);

        // 读取符号名字符串表（从符号表节的link字段读出）
        const Elf64_Shdr* strtab_sh = &sh_table[sh->sh_link];
        char* strtab = malloc(strtab_sh->sh_size);
        fseek(fp, strtab_sh->sh_offset, SEEK_SET);
        fread(strtab, 1, strtab_sh->sh_size, fp);

        // 读取符号表到JSON对象
        // 为当前节创建一个JSON对象，并添加符号表信息
        cJSON* jsymtab = cJSON_CreateObject();
        cJSON_AddStringToObject(jsymtab, "name", (sh->sh_name < shstr_shdr->sh_size) ? (shstrtab + sh->sh_name) : "<invalid>");
        cJSON_AddArrayToObject(jsymtab, "symbols");
        for (int j = 0; j < sym_count; ++j) {
            Elf64_Sym* sym = &symtab[j];
            const char* name = (sym->st_name < strtab_sh->sh_size) ? (strtab + sym->st_name) : "<invalid>";
            cJSON* jsym = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsym, "index", j);
            cJSON_AddStringToObject(jsym, "name", name);
            cJSON_AddNumberToObject(jsym, "value", sym->st_value);
            cJSON_AddNumberToObject(jsym, "size", sym->st_size);
            cJSON_AddNumberToObject(jsym, "info", sym->st_info);
            cJSON_AddNumberToObject(jsym, "other", sym->st_other);
            cJSON_AddNumberToObject(jsym, "shndx", sym->st_shndx);
            // 将符号表项添加到当前节的符号数组中
            cJSON_AddItemToArray(cJSON_GetObjectItem(jsymtab, "symbols"), jsym);
        }
        // 将当前节的符号表添加到总的符号表数组中
        cJSON_AddItemToArray(jsymbols, jsymtab);
        free(symtab);
        free(strtab);
    }

    cJSON_AddItemToObject(root, "symbols", jsymbols);
    free(shstrtab);
    free(sh_table);
    return true;
}
