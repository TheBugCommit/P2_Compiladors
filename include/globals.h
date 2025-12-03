#ifndef GLOBALS_H
#define GLOBALS_H

#include "data_types.h"

#define ADDRESS_SIZE 64

extern const char* TYPE_NAME[];

typedef struct {
    int line;
    int column;
    const char* filename;
} SourceLocation;

SourceLocation current_location(void);

#endif