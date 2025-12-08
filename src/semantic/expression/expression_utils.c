#include "expression_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../codegen/codegen.h"
#include "../../codegen/gen_temps.h"
#include "../../utils/error.h"
#include "../../utils/log.h"
#include "expression_priv.h"
#include "globals.h"

double get_value(ExprResult e) {
    return (e.type == TYPE_FLOAT) ? e.value.float_val : (double)e.value.int_val;
}

void cleanup_operands(ExprResult a, ExprResult b) {
    if (a.addr) free(a.addr);
    if (b.addr) free(b.addr);
}

ExprResult clone_result(ExprResult original) {
    ExprResult copy = original;
    if (original.addr) copy.addr = strdup(original.addr);
    return copy;
}

int is_numeric(ExprResult* e) {
    if ((e->type != TYPE_INT && e->type != TYPE_FLOAT) || e->is_array) {
        if(e->is_array) {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Type error: expected numeric type, got array");
        } else {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Type error: expected numeric type, got %s",
                                  TYPE_NAME[e->type]);
        }
        return 0;
    }
    return 1;
}
ExprResult make_literal_result(DATA_TYPE type, double val) {
    ExprResult res;
    res.type = type;
    res.is_constant = 1;
    res.is_array = 0;
    res.addr = (char*)malloc(ADDRESS_SIZE);

    if (type == TYPE_INT) {
        res.value.int_val = (int)val;
        sprintf(res.addr, "%d", res.value.int_val);
    } else {
        res.value.float_val = (float)val;
        sprintf(res.addr, "%.5g", res.value.float_val);
        if (strchr(res.addr, '.') == NULL && strchr(res.addr, 'e') == NULL &&
            strchr(res.addr, 'E') == NULL) {
            strcat(res.addr, ".0");
        }
    }
    return res;
}

ExprResult ensure_type(ExprResult expr, DATA_TYPE target_type) {
    if (expr.type == target_type) return expr;

    if (expr.type == TYPE_INT && target_type == TYPE_FLOAT) {
        if (expr.is_constant) {
            if (expr.addr) free(expr.addr);
            return make_literal_result(TYPE_FLOAT, (float)expr.value.int_val);
        }

        char* t_conv =
            is_temporary(expr.addr) ? strdup(expr.addr) : gen_new_temp();
        emit_unary("I2F", t_conv, expr.addr);
        if (expr.addr) free(expr.addr);

        expr.addr = t_conv;
        expr.type = TYPE_FLOAT;
    } else {
        if (expr.is_constant) {
            if (expr.addr) free(expr.addr);
            return make_literal_result(TYPE_INT, expr.value.float_val);
        }

        char* t_conv =
            is_temporary(expr.addr) ? strdup(expr.addr) : gen_new_temp();
        emit_unary("F2I", t_conv, expr.addr);
        if (expr.addr) free(expr.addr);

        expr.addr = t_conv;
        expr.type = TYPE_INT;
    }

    expr.is_constant = 0;
    return expr;
}