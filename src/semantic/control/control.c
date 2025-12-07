#include "control.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../codegen/codegen.h"
#include "../../codegen/gen_temps.h"
#include "../../utils/error.h"
#include "../../utils/log.h"
#include "../expression/expression_utils.h"

LoopContext handle_repeat_start(ExprResult count_expr) {
    if (count_expr.type != TYPE_INT) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Repeat expression must be an INTEGER (got %s)",
                              TYPE_NAME[count_expr.type]);
        count_expr = make_literal_result(TYPE_INT, 0);
    }

    if (count_expr.is_constant && count_expr.value.int_val <= 0) {
        REPORT_WARNING(current_location(), "SEMANTIC",
                       "Repeat count is %d (<= 0). Loop body might execute "
                       "once or behave unexpectedly in simple C3A.",
                       count_expr.value.int_val);
    }

    LoopContext ctx;
    ctx.bound_addr = count_expr.addr;
    ctx.counter_addr = gen_new_temp();
    emit_assign(ctx.counter_addr, "0");

    log_production("Control: Repeat Start (Count 0 to %s)", ctx.bound_addr);

    return ctx;
}

void handle_repeat_end(LoopContext ctx, int start_label) {
    emit_binary("ADDI", ctx.counter_addr, ctx.counter_addr, "1");
    emit_if_goto("LTI", ctx.counter_addr, ctx.bound_addr, start_label);
    log_production("Control: Repeat End");

    gen_release_temp(ctx.counter_addr);
    gen_release_temp(ctx.bound_addr);
    if (ctx.counter_addr) free(ctx.counter_addr);
    if (ctx.bound_addr) free(ctx.bound_addr);
}