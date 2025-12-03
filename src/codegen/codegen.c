#include "codegen.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static FILE* out = NULL;
static int line_count = 1;

static void emit(const char* format, ...) {
    va_list args;
    fprintf(out, "%d:\t", line_count++);
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    fprintf(out, "\n");
}

void code_init(FILE* output_file) {
    out = output_file ? output_file : stdout;
    line_count = 1;
}

void code_close() {
    if (out != NULL) {
        fclose(out);
        out = NULL;
    }
    line_count = 1;
}

int gen_get_label() {
    return line_count;
}

void emit_binary(const char* op, const char* dest, const char* op1,
                 const char* op2) {
    emit("%s := %s %s %s", dest, op1, op, op2);
}

void emit_assign(const char* dest, const char* src) {
    emit("%s := %s", dest, src);
}

void emit_io(const char* param, const char* prod, const int args) {
    emit("PARAM %s", param);
    emit("CALL %s, %d", args);
}

void emit_halt() { emit("HALT"); }

void emit_if_goto(const char* op, const char* op1, const char* op2, int label) {
    emit("IF %s %s %s GOTO %d", op1, op, op2, label);
}

void emit_unary(const char* op, const char* dest, const char* op1) {
    emit("%s := %s %s", dest, op, op1);
}

void emit_array_store(const char* array_name, const char* offset, const char* value) {
    emit("%s[%s] := %s", array_name, offset, value);
}