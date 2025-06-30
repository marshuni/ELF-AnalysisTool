#ifndef ELF_HEADER_H
#define ELF_HEADER_H

#include <stdio.h>
#include <stdbool.h>
#include "json/cJSON.h"

// 读取并解析 ELF 文件头，写入 JSON 对象
bool parse_elf_header(FILE* fp, cJSON* root);

#endif
