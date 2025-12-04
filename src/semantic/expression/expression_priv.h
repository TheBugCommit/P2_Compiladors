#ifndef EXPRESSION_PRIV_H
#define EXPRESSION_PRIV_H

#include "data_types.h"
#include "expression.h"

typedef enum { OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_POW } ArithOp;

double get_value(ExprResult e);
void cleanup_operands(ExprResult a, ExprResult b);
ExprResult clone_result(ExprResult original);
int is_temporary(const char* addr);
int is_numeric(ExprResult* e);
ExprResult make_literal_result(DATA_TYPE type, double val);
ExprResult ensure_type(ExprResult expr, DATA_TYPE target_type);

int check_arithmetic_validity(ExprResult* a, ExprResult* b, ArithOp op);

ExprResult try_constant_folding(ExprResult a, ExprResult b, ArithOp op);
ExprResult try_algebraic_optimization(ExprResult a, ExprResult b, ArithOp op);
ExprResult pow_optimization(ExprResult a, ExprResult b);

ExprResult generate_arithmetic(ExprResult a, ExprResult b, char* op_int, char* op_float); 
ExprResult generate_pow_code(ExprResult a, ExprResult b);
ExprResult generate_arithmetic_code(ExprResult a, ExprResult b, ArithOp op, char* opcode_int, char* opcode_float);

#endif