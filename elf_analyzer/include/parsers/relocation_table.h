#ifndef RELOCATION_HEADER_H
#define RELOCATION_HEADER_H

#include <stdio.h>
#include <stdbool.h>
#include "json/cJSON.h"

bool parse_relocation_tables(FILE* fp, cJSON* root);

#endif
