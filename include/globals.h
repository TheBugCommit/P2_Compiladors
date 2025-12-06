#ifndef GLOBALS_H
#define GLOBALS_H

#include "data_types.h"

#define ADDRESS_SIZE 64
#define VAR_BYTES 4

extern const char* TYPE_NAME[];
extern int global_arrays_offset;

typedef struct {
    int line;
    int column;
    const char* filename;
} SourceLocation;

SourceLocation current_location(void);

#endif