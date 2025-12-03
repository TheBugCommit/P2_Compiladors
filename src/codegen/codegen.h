#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

void code_init(FILE* output_file);
void code_close();
int gen_get_label();
void emit_binary(const char* op, const char* dest, const char* op1,
                 const char* op2);
void emit_goto(int label);
void emit_unary(const char* op, const char* dest, const char* op1);
void emit_assign(const char* dest, const char* src);
void emit_io(const char* param, const char* prod, const int args);
void emit_if_goto(const char* op, const char* op1, const char* op2, int label);
void emit_array_store(const char* array_name, const char* offset, const char* value);
void emit_halt();

#endif