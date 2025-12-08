#include <stdlib.h>

#include "../../utils/log.h"
#include "../../codegen/opcodes.h"
#include "expression.h"
#include "expression_priv.h"

static int is_literal_val(ExprResult e, double target) {
    if (!e.is_constant) return 0;
    return get_value(e) == target;
}

ExprResult try_algebraic_optimization(ExprResult a, ExprResult b, ArithOp op) {
    ExprResult res = {TYPE_VOID};

    if (op == OP_ADD) {
        if (is_literal_val(b, 0)) {
            log_production("Algebraic Opt: x + 0 -> x");
            if (b.addr) free(b.addr);
            return a;
        }
        if (is_literal_val(a, 0)) {
            log_production("Algebraic Opt: 0 + x -> x");
            if (a.addr) free(a.addr);
            return b;
        }
    }

    if (op == OP_SUB) {
        if (is_literal_val(b, 0)) {
            log_production("Algebraic Opt: x - 0 -> x");
            if (b.addr) free(b.addr);
            return a;
        }
    }

    if (op == OP_MUL) {
        if (is_literal_val(b, 1)) {
            log_production("Algebraic Opt: x * 1 -> x");
            if (b.addr) free(b.addr);
            return a;
        }
        if (is_literal_val(a, 1)) {
            log_production("Algebraic Opt: 1 * x -> x");
            if (a.addr) free(a.addr);
            return b;
        }

        if (is_literal_val(b, 0) || is_literal_val(a, 0)) {
            log_production("Algebraic Opt: x * 0 -> 0");
            cleanup_operands(a, b);
            DATA_TYPE t = (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT)
                              ? TYPE_FLOAT
                              : TYPE_INT;
            return make_literal_result(t, 0);
        }

        if (is_literal_val(b, 2)) {
            log_production("Strength Reduction: x * 2 -> x + x");
            if (b.addr) free(b.addr);
            ExprResult a_copy = clone_result(a);
            return eval_add(a, a_copy);
        }

        if (is_literal_val(a, 2)) {
            log_production("Strength Reduction: 2 * x -> x + x");
            if (a.addr) free(a.addr);
            ExprResult b_copy = clone_result(b);
            return eval_add(b, b_copy);
        }
    }

    if (op == OP_DIV) {
        if (is_literal_val(b, 1)) {
            log_production("Algebraic Opt: x / 1 -> x");
            if (b.addr) free(b.addr);
            return a;
        }
    }

    if (op == OP_MOD) {
        if (is_literal_val(b, 1)) {
            log_production("Algebraic Opt: x % 1 -> 0");
            cleanup_operands(a, b);
            return make_literal_result(TYPE_INT, 0);
        }
    }

    if (op == OP_POW) {
        if (is_literal_val(b, 0)) {
            log_production("Algebraic Opt: x ** 0 -> 1");
            cleanup_operands(a, b);
            DATA_TYPE t = (a.type == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT;
            return make_literal_result(t, 1);
        }

        if (is_literal_val(b, 1)) {
            log_production("Algebraic Opt: x ** 1 -> x");
            if (b.addr) free(b.addr);
            return a;
        }
    }

    return res;
}

ExprResult pow_optimization(ExprResult a, ExprResult b) {
    ExprResult res;
    int n = b.value.int_val;
    log_production("Optimization: Loop Unrolling POW (n=%d)", n);

    if (n == 0) {
        cleanup_operands(a, b);
        DATA_TYPE t = (a.type == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT;
        return make_literal_result(t, 1);
    }
    if (n == 1) {
        if (b.addr) free(b.addr);
        return a;
    }

    res = clone_result(a);
    for (int i = 1; i < n; i++) {
        ExprResult next_a = clone_result(a);
        res = generate_arithmetic(res, next_a, OC_MULI, OC_MULF);
    }

    cleanup_operands(a, b);
    return res;
}