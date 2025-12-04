#include "expression_priv.h"
#include <math.h>
#include "../../utils/log.h"

ExprResult try_constant_folding(ExprResult a, ExprResult b, ArithOp op) {
    ExprResult res = {TYPE_VOID};

    if (!a.is_constant || !b.is_constant) return res;

    log_production("Optimization: Constant Folding applied");

    double val_a = get_value(a);
    double val_b = get_value(b);
    double result_val = 0;

    switch (op) {
        case OP_ADD: result_val = val_a + val_b; break;
        case OP_SUB: result_val = val_a - val_b; break;
        case OP_MUL: result_val = val_a * val_b; break;
        case OP_DIV: result_val = val_a / val_b; break;
        case OP_MOD: result_val = (int)val_a % (int)val_b; break;
        case OP_POW: result_val = pow(val_a, val_b); break;
    }

    DATA_TYPE res_type = TYPE_INT;
    if (op != OP_MOD) {
        if (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) res_type = TYPE_FLOAT;
        else if (op == OP_POW && val_b < 0) res_type = TYPE_FLOAT;
    }

    cleanup_operands(a, b);
    
    return make_literal_result(res_type, result_val);
}