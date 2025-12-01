#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

void code_init(FILE *output_file);
void code_close();
// Genera una operació binària: res := op1 OPERATOR op2
// Ex: emit_binary("ADDI", "$t01", "var", "5") -> "N: $t01 := var ADDI 5"
void emit_binary(const char *op, const char *dest, const char *op1, const char *op2);
// Genera una assignació/còpia: dest := src
void emit_assign(const char *dest, const char *src);
// Genera operacions d'IO (PUT)
void emit_io(const char *param, const char *prod, const int args);

void emit_halt();

#endif