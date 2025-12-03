#include "expression.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../codegen/codegen.h"
#include "../codegen/gen_temps.h"
#include "../utils/error.h"
#include "../utils/log.h"
#include "globals.h"

typedef enum { OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_POW } ArithOp;

static double get_value(ExprResult e) {
    return (e.type == TYPE_FLOAT) ? e.value.float_val : (double)e.value.int_val;
}

static void cleanup_operands(ExprResult a, ExprResult b) {
    if (a.addr) free(a.addr);
    if (b.addr) free(b.addr);
}

static ExprResult clone_result(ExprResult original) {
    ExprResult copy = original;
    if (original.addr) {
        copy.addr = strdup(original.addr);
    }
    return copy;
}

int is_numeric(ExprResult* e) {
    if (e->type != TYPE_INT && e->type != TYPE_FLOAT) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Type error: expected numeric type, got %s",
                              TYPE_NAME[e->type]);
        return 0;
    }
    return 1;
}

static ExprResult make_literal_result(DATA_TYPE type, double val) {
    ExprResult res;
    res.type = type;
    res.is_constant = 1;
    res.addr = (char*)malloc(ADDRESS_SIZE);

    if (type == TYPE_INT) {
        res.value.int_val = (int)val;
        sprintf(res.addr, "%d", res.value.int_val);
    } else {
        res.value.float_val = (float)val;
        sprintf(res.addr, "%g", res.value.float_val);
    }
    return res;
}


static ExprResult ensure_type(ExprResult expr, DATA_TYPE target_type) {
    if (expr.type == target_type) return expr;

    if (expr.type == TYPE_INT && target_type == TYPE_FLOAT) {
        if (expr.is_constant) {
            float val = (float)expr.value.int_val;

            if (expr.addr) free(expr.addr);
            expr.addr = malloc(ADDRESS_SIZE);
            sprintf(expr.addr, "%g", val);

            expr.type = TYPE_FLOAT;
            expr.value.float_val = val;
            return expr;
        }

        char* t_conv = gen_new_temp();
        emit_unary("I2F", t_conv, expr.addr);

        gen_release_temp(expr.addr);
        if (expr.addr) free(expr.addr);

        expr.addr = t_conv;
        expr.type = TYPE_FLOAT;
        expr.is_constant = 0;
        return expr;
    }

    return expr;
}

static ExprResult generate_arithmetic(ExprResult a, ExprResult b, char* op_int,
                                      char* op_float) {
    ExprResult res;

    int is_float = (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT);
    res.type = is_float ? TYPE_FLOAT : TYPE_INT;
    char* opcode = is_float ? op_float : op_int;

    if (is_float) {
        a = ensure_type(a, TYPE_FLOAT);
        b = ensure_type(b, TYPE_FLOAT);
    }

    if (is_temporary(a.addr)) {  // reuse left op
        res.addr = a.addr;
        emit_binary(opcode, res.addr, a.addr, b.addr);
        gen_release_temp(b.addr);
        if (b.addr) free(b.addr);
    } else if (is_temporary(b.addr)) {  // reuse right op
        res.addr = b.addr;
        emit_binary(opcode, res.addr, a.addr, b.addr);
        gen_release_temp(a.addr);
        if (a.addr) free(a.addr);
    } else {
        res.addr = gen_new_temp();
        emit_binary(opcode, res.addr, a.addr, b.addr);
        gen_release_temp(a.addr);
        gen_release_temp(b.addr);
        cleanup_operands(a, b);
    }

    res.is_constant = 0;

    return res;
}

static ExprResult generate_pow_code(ExprResult a, ExprResult b) {
    ExprResult res = {TYPE_VOID};

    if (b.is_constant && b.value.int_val >= 0 && b.value.int_val <= 4) {
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
        char* op_int = "MULI"; char* op_float = "MULF";

        for (int i = 1; i < n; i++) {
            ExprResult next_a = clone_result(a);
            res = generate_arithmetic(res, next_a, op_int, op_float);
        }
        
        cleanup_operands(a, b);
        return res;
    }

    log_production("Generating runtime loop for POW");
    
    char* t_res = gen_new_temp();
    char* t_cnt = gen_new_temp();

    if (a.type == TYPE_FLOAT) { emit_assign(t_res, "1.0"); res.type = TYPE_FLOAT; }
    else { emit_assign(t_res, "1"); res.type = TYPE_INT; }
    
    emit_assign(t_cnt, "0");

    int start_label = gen_get_label(); 

    emit_if_goto("GEI", t_cnt, b.addr, start_label + 4);

    char* op_mul = (res.type == TYPE_FLOAT) ? "MULF" : "MULI";
    emit_binary(op_mul, t_res, t_res, a.addr);
    emit_binary("ADDI", t_cnt, t_cnt, "1");
    emit_goto(start_label);
    cleanup_operands(a, b);
    gen_release_temp(t_cnt); 
    if(t_cnt) free(t_cnt);

    res.is_constant = 0;
    res.addr = t_res;
    return res;
}

static ExprResult eval_arithmetic_common(ExprResult a, ExprResult b, ArithOp op,
                                         char* name, char* op_int,
                                         char* op_float) {
    ExprResult res = {TYPE_VOID};

    if (!is_numeric(&a) || !is_numeric(&b)) return res;

    if (op == OP_POW && (b.type == TYPE_FLOAT)) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Pow invalid FLOAT exponent");
        cleanup_operands(a, b);
        return res;
    }

    if (op == OP_MOD && (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT)) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Modulo (%) invalid for FLOAT operands");
        cleanup_operands(a, b);
        return res;
    }

    if ((op == OP_DIV || op == OP_MOD) && b.is_constant) {
        double val_b = get_value(b);
        if (val_b == 0) {
            REPORT_SEMANTIC_ERROR(current_location(), "%s by zero detected",
                                  name);
            cleanup_operands(a, b);
            return res;
        }
    }

    if (a.is_constant && b.is_constant) {
        log_production("Optimization: Constant Folding %s", name);

        cleanup_operands(a, b);

        double val_a = get_value(a);
        double val_b = get_value(b);
        double result_val = 0;

        switch (op) {
            case OP_ADD:
                result_val = val_a + val_b;
                break;
            case OP_SUB:
                result_val = val_a - val_b;
                break;
            case OP_MUL:
                result_val = val_a * val_b;
                break;
            case OP_DIV:
                result_val = val_a / val_b;
                break;
            case OP_MOD:
                result_val = (int)val_a % (int)val_b;
                break;
            case OP_POW: 
                result_val = pow(val_a, val_b); 
                break;
        }

        DATA_TYPE res_type = TYPE_INT;
        if (op != OP_MOD) {
            if (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) {
                res_type = TYPE_FLOAT;
            } else if (op == OP_POW && val_b < 0) {
                res_type = TYPE_FLOAT;
            }
        }

        return make_literal_result(res_type, result_val);
    }

    log_production("Generating code for %s", name);

    if (op == OP_POW) {
        return generate_pow_code(a, b);
    } else {
        return generate_arithmetic(a, b, op_int, op_float);
    }
}

ExprResult eval_add(ExprResult a, ExprResult b) {
    return eval_arithmetic_common(a, b, OP_ADD, "ADD", "ADDI", "ADDF");
}

ExprResult eval_sub(ExprResult a, ExprResult b) {
    return eval_arithmetic_common(a, b, OP_SUB, "SUB", "SUBI", "SUBF");
}

ExprResult eval_mul(ExprResult a, ExprResult b) {
    return eval_arithmetic_common(a, b, OP_MUL, "MUL", "MULI", "MULF");
}

ExprResult eval_div(ExprResult a, ExprResult b) {
    return eval_arithmetic_common(a, b, OP_DIV, "DIV", "DIVI", "DIVF");
}

ExprResult eval_mod(ExprResult a, ExprResult b) {
    // MODI no té versió float, passem NULL o "" com a op_float
    return eval_arithmetic_common(a, b, OP_MOD, "MOD", "MODI", "");
}

ExprResult eval_pow(ExprResult a, ExprResult b) {
    return eval_arithmetic_common(a, b, OP_POW, "POW", "", "");
}

ExprResult eval_unary_minus(ExprResult a) {
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
    char* op_code = (a.type == TYPE_FLOAT) ? "CHSF" : "CHSI";

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

ExprResult eval_unary_plus(ExprResult a) { return a; }