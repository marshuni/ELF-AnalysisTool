#ifndef SECTION_HEADER_H
#define SECTION_HEADER_H

#include <stdio.h>
#include <stdbool.h>
#include "json/cJSON.h"

bool parse_section_headers(FILE* fp, cJSON* root);

#endif
