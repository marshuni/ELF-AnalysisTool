#ifndef SYNAMIC_SEGMENT_H
#define SYNAMIC_SEGMENT_H

#include <stdio.h>
#include <stdbool.h>
#include "json/cJSON.h"

bool parse_dynamic_segment(FILE* fp, cJSON* root);

#endif
