#include "handlers.h"
#include <stdlib.h>
#include "../../codegen/codegen.h"
#include "../../codegen/gen_temps.h"
#include "../../utils/error.h"
#include "../../utils/log.h"
#include "../expression/expression_utils.h"

void handle_put_statement(ExprResult expr) {
    if (expr.type != TYPE_INT && expr.type != TYPE_FLOAT) {
        REPORT_SEMANTIC_ERROR(current_location(), 
            "Cannot put expression of type %s. Only INT and FLOAT are supported.", 
            TYPE_NAME[expr.type]);
        if(expr.addr) free(expr.addr);
        return;
    }

    log_production("IO: Generating PUT for %s expression", TYPE_NAME[expr.type]);

    const char* proc_name = (expr.type == TYPE_INT) ? "PUTI" : "PUTF";

    emit_io(expr.addr, proc_name, 1);

    gen_release_temp(expr.addr);
    if(expr.addr) free(expr.addr);
}