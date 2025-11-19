#ifndef DECLARATION_H
#define DECLARATION_H

#include "data_types.h"

int is_numeric(ExprResult *e);
int is_int(ExprResult *e);
ExprResult eval_add(ExprResult a, ExprResult b);
ExprResult eval_sub(ExprResult a, ExprResult b);
ExprResult eval_mul(ExprResult a, ExprResult b);
ExprResult eval_div(ExprResult a, ExprResult b);
ExprResult eval_mod(ExprResult a, ExprResult b);
ExprResult eval_pow(ExprResult a, ExprResult b);
ExprResult eval_unary_minus(ExprResult a);
ExprResult eval_unary_plus(ExprResult a);

#endif