#include "parsers/dynamic_segment.h"
#include "elf/elf.h"
#include <stdlib.h>
#include <string.h>

bool parse_dynamic_segment(FILE* fp, cJSON* root) {
    fseek(fp, 0, SEEK_SET);
    Elf64_Ehdr ehdr;
    if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) return false;

    fseek(fp, ehdr.e_shoff, SEEK_SET);
    Elf64_Shdr* sh_table = (Elf64_Shdr*)malloc(ehdr.e_shentsize * ehdr.e_shnum);
    if (!sh_table) return false;
    fread(sh_table, ehdr.e_shentsize, ehdr.e_shnum, fp);

    cJSON* jdynamic = cJSON_CreateArray();

    for (int i = 0; i < ehdr.e_shnum; ++i) {
        const Elf64_Shdr* sh = &sh_table[i];
        if (sh->sh_type != SHT_DYNAMIC) continue;

        Elf64_Dyn* dyns = (Elf64_Dyn*)malloc(sh->sh_size);
        if (!dyns) continue;
        fseek(fp, sh->sh_offset, SEEK_SET);
        fread(dyns, 1, sh->sh_size, fp);

        int dyn_count = sh->sh_size / sizeof(Elf64_Dyn);
        for (int j = 0; j < dyn_count; ++j) {
            Elf64_Dyn* dyn = &dyns[j];
            cJSON* jentry = cJSON_CreateObject();
            cJSON_AddNumberToObject(jentry, "tag", dyn->d_tag);
            cJSON_AddNumberToObject(jentry, "val_ptr", dyn->d_un.d_val);
            cJSON_AddItemToArray(jdynamic, jentry);
        }
        free(dyns);
    }

    cJSON_AddItemToObject(root, "dynamic_segment", jdynamic);
    free(sh_table);
    return true;
}