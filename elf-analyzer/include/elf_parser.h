#ifndef ELF_PARSER_H
#define ELF_PARSER_H

#include <stdbool.h>

bool parse_elf_file(const char* filepath, const char* output_path);

#endif
