#ifndef PROGRAM_HEADER_H
#define PROGRAM_HEADER_H

#include <stdio.h>
#include <stdbool.h>
#include "json/cJSON.h"

// 解析 ELF 的 Program Header 表
bool parse_program_header(FILE* fp, cJSON* root);

#endif
