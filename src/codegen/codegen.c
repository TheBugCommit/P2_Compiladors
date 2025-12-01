#include "codegen.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static FILE* out = NULL;
static int line_count = 1;

void code_init(FILE* output_file) {
    out = output_file ? output_file : stdout;
    line_count = 1;
}

void code_close() {
    if (out != NULL) {
        fclose(out);
        out = NULL;
    }

    line_count = 0;
}

static void emit_instr(const char* format, ...) {
    va_list args;
    fprintf(out, "%d:\n\t", line_count++);
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    fprintf(out, "\n");
}

void emit_binary(const char* op, const char* dest, const char* op1,
                 const char* op2) {
    emit_instr("%s := %s %s %s", dest, op1, op, op2);
}

void emit_assign(const char* dest, const char* src) {
    emit_instr("%s := %s", dest, src);
}

void emit_io(const char* param, const char* prod, const int args) {
    emit_instr("PARAM %s", param);
    emit_instr("CALL %s, %d", args);
}

void emit_halt() { emit_instr("HALT"); }