#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>

#include "globals.h"

typedef enum {
    ERROR_LEVEL_NONE,
    ERROR_LEVEL_WARNING,
    ERROR_LEVEL_ERROR,
    ERROR_LEVEL_FATAL
} ErrorLevel;

typedef struct {
    ErrorLevel level;
    char message[256];
    SourceLocation location;
    const char* module;
} ErrorInfo;

void error_init(void);
void error_cleanup(void);
int get_error_count(void);

void report_error(ErrorLevel level, SourceLocation loc, const char* module,
                  const char* format, ...);

void yyerror(const char* s);

bool has_errors(void);
bool has_fatal_errors(void);
ErrorLevel get_max_error_level(void);

SourceLocation current_location(void);

#define REPORT_LEXICAL_ERROR(loc, ...) \
    report_error(ERROR_LEVEL_ERROR, loc, "LEXER", __VA_ARGS__);

#define REPORT_SYNTAX_ERROR(loc, ...) \
    report_error(ERROR_LEVEL_ERROR, loc, "PARSER", __VA_ARGS__);

#define REPORT_SEMANTIC_ERROR(loc, ...) \
    report_error(ERROR_LEVEL_ERROR, loc, "SEMANTIC", __VA_ARGS__);

#define REPORT_WARNING(loc, module, ...) \
    report_error(ERROR_LEVEL_WARNING, loc, module, __VA_ARGS__);

#endif