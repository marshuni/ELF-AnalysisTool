#include <stdio.h>
#include "elf_parser.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: elf-analyzer <elf_file> [output.json]\n");
        return 1;
    }

    const char* elf_file = argv[1];
    const char* output_file = (argc >= 3) ? argv[2] : NULL;

    if (!parse_elf_file(elf_file, output_file)) {
        fprintf(stderr, "Failed to parse ELF file.\n");
        return 1;
    }

    printf("ELF parsing completed.\n");
    return 0;
}