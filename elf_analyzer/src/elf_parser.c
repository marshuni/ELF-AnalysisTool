#include "elf_parser.h"
#include "parsers/elf_header.h"
#include "parsers/program_header.h"
#include "parsers/section_header.h"
// 其他模块
#include "json/cJSON.h"

#include <stdio.h>
#include <stdlib.h>

bool parse_elf_file(const char* filepath, const char* output_path) {
    FILE* fp = fopen(filepath, "rb");
    if (!fp) {
        perror("fopen");
        return false;
    }

    cJSON* root = cJSON_CreateObject();

    if (!parse_elf_header(fp, root)) {
        fclose(fp);
        cJSON_Delete(root);
        return false;
    }

    if (!parse_program_header(fp, root)) {
        fclose(fp);
        cJSON_Delete(root);
        return false;
    }

    if (!parse_section_headers(fp, root)) {
        fclose(fp);
        cJSON_Delete(root);
        return false;
    }
    // 其他模块：parse_program_header(fp, root) 等

    if (output_path) {
        FILE* out = fopen(output_path, "w");
        if (!out) {
            perror("fopen (output)");
            fclose(fp);
            cJSON_Delete(root);
            return false;
        }

        char* json_str = cJSON_Print(root);
        fprintf(out, "%s\n", json_str);
        free(json_str);
        fclose(out);
    } else {
        char* json_str = cJSON_Print(root);
        printf("%s\n", json_str);
        free(json_str);
    }

    fclose(fp);
    cJSON_Delete(root);
    return true;
}
