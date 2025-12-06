#ifndef EXPRESSION_PRIV_H
#define EXPRESSION_PRIV_H

#include "data_types.h"
#include "expression.h"
#include "expression_utils.h"

typedef enum { OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_POW } ArithOp;

int check_arithmetic_validity(ExprResult* a, ExprResult* b, ArithOp op);

ExprResult try_constant_folding(ExprResult a, ExprResult b, ArithOp op);
ExprResult try_algebraic_optimization(ExprResult a, ExprResult b, ArithOp op);
ExprResult pow_optimization(ExprResult a, ExprResult b);

ExprResult generate_arithmetic(ExprResult a, ExprResult b, char* op_int, char* op_float); 
ExprResult generate_pow_code(ExprResult a, ExprResult b);
ExprResult generate_arithmetic_code(ExprResult a, ExprResult b, ArithOp op, char* opcode_int, char* opcode_float);

#endif