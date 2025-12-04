#include "expression_priv.h"
#include <stdlib.h>
#include "../../utils/error.h"

int check_arithmetic_validity(ExprResult* a, ExprResult* b, ArithOp op) {
    if (!is_numeric(a) || !is_numeric(b)) return 0;

    if (op == OP_MOD && (a->type == TYPE_FLOAT || b->type == TYPE_FLOAT)) {
        REPORT_SEMANTIC_ERROR(current_location(), "Modulo (%) invalid for FLOAT operands");
        return 0;
    }

    if (op == OP_POW && b->type == TYPE_FLOAT) {
        REPORT_SEMANTIC_ERROR(current_location(), "Pow invalid FLOAT exponent (runtime)");
        return 0;
    }

    if ((op == OP_DIV || op == OP_MOD) && b->is_constant) {
        double val_b = get_value(*b);
        if (val_b == 0) {
            REPORT_SEMANTIC_ERROR(current_location(), "Division/Modulo by zero detected");
            return 0;
        }
    }

    return 1;
}