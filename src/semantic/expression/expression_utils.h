#ifndef EXPRESSION_UTILS_H
#define EXPRESSION_UTILS_H

#include "data_types.h"
#include "expression.h"

double get_value(ExprResult e);
void cleanup_operands(ExprResult a, ExprResult b);
ExprResult clone_result(ExprResult original);
int is_temporary(const char* addr);
int is_numeric(ExprResult* e);
ExprResult make_literal_result(DATA_TYPE type, double val);
ExprResult ensure_type(ExprResult expr, DATA_TYPE target_type);

#endif