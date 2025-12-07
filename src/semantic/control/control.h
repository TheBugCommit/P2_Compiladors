#ifndef CONTROL_H
#define CONTROL_H

#include "../expression/expression.h"

ExprResult handle_repeat_start(ExprResult count_expr);
void handle_repeat_end(ExprResult counter, int start_label);

#endif