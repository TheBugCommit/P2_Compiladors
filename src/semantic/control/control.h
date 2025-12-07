#ifndef CONTROL_H
#define CONTROL_H

#include "../expression/expression.h"

typedef struct {
    char* counter_addr;
    char* bound_addr;  
} LoopContext;

LoopContext handle_repeat_start(ExprResult count_expr);
void handle_repeat_end(LoopContext ctx, int start_label);

#endif