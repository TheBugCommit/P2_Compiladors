#include "control.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../codegen/codegen.h"
#include "../../codegen/gen_temps.h"
#include "../../utils/error.h"
#include "../../utils/log.h"
#include "../expression/expression_utils.h"

ExprResult handle_repeat_start(ExprResult count_expr) {
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

    char* counter_temp = gen_new_temp();
    emit_assign(counter_temp, count_expr.addr);

    ExprResult res = {TYPE_INT};
    res.addr = counter_temp;
    res.is_constant = 0;

    if (count_expr.addr) {
        gen_release_temp(count_expr.addr);
        free(count_expr.addr);
    }

    return res;
}

void handle_repeat_end(ExprResult counter, int start_label) {
    log_production("Control: Repeat Loop Closed (Jumping to %d)", start_label);

    emit_binary("SUBI", counter.addr, counter.addr, "1");
    emit_if_goto("GEI", counter.addr, "1", start_label);
    
    gen_release_temp(counter.addr);
    if(counter.addr) free(counter.addr);
}