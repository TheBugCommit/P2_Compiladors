#include "expression.h"

#include <stdlib.h>

#include "../../codegen/codegen.h"
#include "../../codegen/gen_temps.h"
#include "../../utils/log.h"
#include "expression_priv.h"
#include "globals.h"

static ExprResult eval_arithmetic_pipeline(ExprResult a, ExprResult b,
                                           ArithOp op, char* op_int,
                                           char* op_float) {
    ExprResult res = {TYPE_VOID};

    if (!check_arithmetic_validity(&a, &b, op)) {
        cleanup_operands(a, b);
        return res;
    }

    res = try_constant_folding(a, b, op);
    if (res.type != TYPE_VOID) return res;

    res = try_algebraic_optimization(a, b, op);
    if (res.type != TYPE_VOID) return res;

    return generate_arithmetic_code(a, b, op, op_int, op_float);
}

ExprResult eval_add(ExprResult a, ExprResult b) {
    log_production("Evaluating ADD");
    return eval_arithmetic_pipeline(a, b, OP_ADD, OC_ADDI, OC_ADDF);
}
ExprResult eval_sub(ExprResult a, ExprResult b) {
    log_production("Evaluating SUB");
    return eval_arithmetic_pipeline(a, b, OP_SUB, OC_SUBI, OC_SUBF);
}
ExprResult eval_mul(ExprResult a, ExprResult b) {
    log_production("Evaluating MUL");
    return eval_arithmetic_pipeline(a, b, OP_MUL, OC_MULI, OC_MULF);
}
ExprResult eval_div(ExprResult a, ExprResult b) {
    log_production("Evaluating DIV");
    return eval_arithmetic_pipeline(a, b, OP_DIV, OC_DIVI, OC_DIVF);
}
ExprResult eval_mod(ExprResult a, ExprResult b) {
    log_production("Evaluating MOD");
    return eval_arithmetic_pipeline(a, b, OP_MOD, OC_MODI, "");
}
ExprResult eval_pow(ExprResult a, ExprResult b) {
    log_production("Evaluating POW");
    return eval_arithmetic_pipeline(a, b, OP_POW, "", "");
}

ExprResult eval_cast(DATA_TYPE target_type, ExprResult expr) {
    if (expr.type == TYPE_VOID) return expr;

    if (!is_numeric(&expr)) {
        if (expr.addr) free(expr.addr);
        ExprResult err = {TYPE_VOID};
        return err;
    }

    log_production("Explicit Cast: Converting %s to %s", TYPE_NAME[expr.type],
                   TYPE_NAME[target_type]);

    return ensure_type(expr, target_type);
}

ExprResult eval_unary_minus(ExprResult a) {
    log_production("Evaluating Unary minus");
    ExprResult res = {TYPE_VOID};
    if (!is_numeric(&a)) {
        if (a.addr) free(a.addr);
        return res;
    }

    if (a.is_constant) {
        log_production("Optimization: Constant Folding Unary Minus");
        if (a.addr) free(a.addr);

        if (a.type == TYPE_FLOAT)
            return make_literal_result(TYPE_FLOAT, -a.value.float_val);
        else
            return make_literal_result(TYPE_INT, (double)-a.value.int_val);
    }

    res.type = a.type;
    res.is_constant = 0;
    char* op_code = (a.type == TYPE_FLOAT) ? OC_CHSF : OC_CHSI;

    if (is_temporary(a.addr)) {
        res.addr = a.addr;
        emit_unary(op_code, res.addr, res.addr);
    } else {
        res.addr = gen_new_temp();
        emit_unary(op_code, res.addr, a.addr);
        if (a.addr) free(a.addr);
    }

    return res;
}

ExprResult eval_unary_plus(ExprResult a) { log_production("Evaluating Unary plus"); return a; }