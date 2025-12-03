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

static ExprResult make_literal_result(DATA_TYPE type, double val) {
    ExprResult res;
    res.type = type;
    res.is_literal = 1;
    res.addr = (char*)malloc(64);

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
        if (expr.is_literal) {
            float val = (float)expr.value.int_val;

            if (expr.addr) free(expr.addr);
            expr.addr = malloc(64);
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
        expr.is_literal = 0;
        return expr;
    }

    return expr;
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

static ExprResult generate_arithmetic(ExprResult a, ExprResult b, char* op_int,
                                      char* op_float) {
    ExprResult res;

    res.addr = gen_new_temp();
    if (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) {
        res.type = TYPE_FLOAT;

        a = ensure_type(a, TYPE_FLOAT);
        b = ensure_type(b, TYPE_FLOAT);

        emit_binary(op_float, res.addr, a.addr, b.addr);
    } else {
        res.type = TYPE_INT;
        emit_binary(op_int, res.addr, a.addr, b.addr);
    }

    res.is_literal = 0;

    gen_release_temp(a.addr);
    gen_release_temp(b.addr);
    if (a.addr) free(a.addr);
    if (b.addr) free(b.addr);

    return res;
}

ExprResult eval_add(ExprResult a, ExprResult b) {
    ExprResult res = {TYPE_VOID};
    if (!is_numeric(&a) || !is_numeric(&b)) return res;

    if (a.is_literal && b.is_literal) {
        log_production("Optimization: Constant Folding ADD");
        if (a.addr) free(a.addr);
        if (b.addr) free(b.addr);

        if (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) {
            float val_a = (a.type == TYPE_FLOAT) ? a.value.float_val
                                                 : (float)a.value.int_val;
            float val_b = (b.type == TYPE_FLOAT) ? b.value.float_val
                                                 : (float)b.value.int_val;
            return make_literal_result(TYPE_FLOAT, val_a + val_b);
        } else {
            return make_literal_result(
                TYPE_INT, (double)(a.value.int_val + b.value.int_val));
        }
    }

    log_production("Generating code for ADD");
    return generate_arithmetic(a, b, "ADDI", "ADDF");
}

ExprResult eval_sub(ExprResult a, ExprResult b) {
    ExprResult res = {TYPE_VOID};
    if (!is_numeric(&a) || !is_numeric(&b)) return res;

    if (a.is_literal && b.is_literal) {
        log_production("Optimization: Constant Folding SUB");
        if (a.addr) free(a.addr);
        if (b.addr) free(b.addr);

        if (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) {
            float val_a = (a.type == TYPE_FLOAT) ? a.value.float_val
                                                 : (float)a.value.int_val;
            float val_b = (b.type == TYPE_FLOAT) ? b.value.float_val
                                                 : (float)b.value.int_val;
            return make_literal_result(TYPE_FLOAT, val_a - val_b);
        } else {
            return make_literal_result(
                TYPE_INT, (double)(a.value.int_val - b.value.int_val));
        }
    }

    log_production("Generating code for SUB");
    return generate_arithmetic(a, b, "SUBI", "SUBF");
}

ExprResult eval_mul(ExprResult a, ExprResult b) {
    ExprResult res = {TYPE_VOID};
    if (!is_numeric(&a) || !is_numeric(&b)) return res;

    if (a.is_literal && b.is_literal) {
        log_production("Optimization: Constant Folding MUL");
        if (a.addr) free(a.addr);
        if (b.addr) free(b.addr);

        if (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) {
            float val_a = (a.type == TYPE_FLOAT) ? a.value.float_val
                                                 : (float)a.value.int_val;
            float val_b = (b.type == TYPE_FLOAT) ? b.value.float_val
                                                 : (float)b.value.int_val;
            return make_literal_result(TYPE_FLOAT, val_a * val_b);
        } else {
            return make_literal_result(
                TYPE_INT, (double)(a.value.int_val * b.value.int_val));
        }
    }

    log_production("Generating code for MUL");
    return generate_arithmetic(a, b, "MULI", "MULF");
}

ExprResult eval_div(ExprResult a, ExprResult b) {
    ExprResult res = {TYPE_VOID};
    if (!is_numeric(&a) || !is_numeric(&b)) return res;

    if (b.is_literal) {
        float val_b =
            (b.type == TYPE_FLOAT) ? b.value.float_val : (float)b.value.int_val;
        if (val_b == 0) {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Division by zero detected");
            if (a.addr) free(a.addr);
            if (b.addr) free(b.addr);
            return res;
        }
    }

    if (a.is_literal && b.is_literal) {
        log_production("Optimization: Constant Folding DIV");
        if (a.addr) free(a.addr);
        if (b.addr) free(b.addr);

        if (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) {
            float val_a = (a.type == TYPE_FLOAT) ? a.value.float_val
                                                 : (float)a.value.int_val;
            float val_b = (b.type == TYPE_FLOAT) ? b.value.float_val
                                                 : (float)b.value.int_val;
            return make_literal_result(TYPE_FLOAT, val_a / val_b);
        } else {
            return make_literal_result(
                TYPE_INT, (double)(a.value.int_val / b.value.int_val));
        }
    }

    log_production("Generating code for DIV");
    return generate_arithmetic(a, b, "DIVI", "DIVF");
}

// TODO: MOD with float operands
ExprResult eval_mod(ExprResult a, ExprResult b) {
    ExprResult res = {TYPE_VOID};
    if (!is_numeric(&a) || !is_numeric(&b)) return res;

    if (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Modulo (%) invalid for FLOAT operands");
        return res;
    }

    if (a.is_literal && b.is_literal) {
        log_production("Optimization: Constant Folding MOD");
        if (a.addr) free(a.addr);
        if (b.addr) free(b.addr);

        if (b.value.int_val == 0) {
            REPORT_SEMANTIC_ERROR(current_location(), "Modulo by zero");
            return res;
        }

        return make_literal_result(TYPE_INT,
                                   (double)(a.value.int_val % b.value.int_val));
    }

    res.type = TYPE_INT;
    res.addr = gen_new_temp();
    res.is_literal = 0;

    emit_binary("MODI", res.addr, a.addr, b.addr);

    gen_release_temp(a.addr);
    gen_release_temp(b.addr);
    if (a.addr) free(a.addr);
    if (b.addr) free(b.addr);
    return res;
}

ExprResult eval_unary_minus(ExprResult a) {
    ExprResult res = {TYPE_VOID};
    if (!is_numeric(&a)) {
        if (a.addr) free(a.addr);
        return res;
    }
    if (a.is_literal) {
        log_production("Optimization: Constant Folding Unary Minus");
        if (a.addr) free(a.addr);

        if (a.type == TYPE_FLOAT)
            return make_literal_result(TYPE_FLOAT, -a.value.float_val);
        else
            return make_literal_result(TYPE_INT, (double)-a.value.int_val);
    }

    res.type = a.type;
    res.is_literal = 0;
    res.addr = gen_new_temp();

    char* op_code = (a.type == TYPE_FLOAT) ? "CHSF" : "CHSI";
    emit_unary(op_code, res.addr, a.addr);

    gen_release_temp(a.addr);
    if(a.addr) free(a.addr);
    return res;
}

ExprResult eval_unary_plus(ExprResult a) {
    return a;
}

//TODO: revisar i comporvar leaks de memoria
ExprResult eval_pow(ExprResult a, ExprResult b) {
    ExprResult res;
    if (!is_numeric(&a) || !is_numeric(&b)) {
        res.type = TYPE_VOID;
        return res;
    }

    // 1. Constant Folding
    if (a.is_literal && b.is_literal) {
        double base =
            (a.type == TYPE_FLOAT) ? a.value.float_val : a.value.int_val;
        double exp =
            (b.type == TYPE_FLOAT) ? b.value.float_val : b.value.int_val;
        double r = pow(base, exp);
        return make_literal_result(
            (a.type == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT, r);
    }

    // 2. Generació: Exponent ha de ser enter per fer bucle simple
    if (b.type != TYPE_INT) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Exponent must be INT for code generation");
        res.type = TYPE_VOID;
        return res;
    }

    // Temporals pel bucle
    char* t_res = gen_new_temp();  // Acumulador result
    char* t_cnt = gen_new_temp();  // Comptador

    // Inicialització: res := 1
    if (a.type == TYPE_FLOAT) {
        emit_assign(t_res, "1.0");
        res.type = TYPE_FLOAT;
    } else {
        emit_assign(t_res, "1");
        res.type = TYPE_INT;
    }
    // Inicialització: cnt := 0
    emit_assign(t_cnt, "0");

    /*
     * LÒGICA DEL BUCLE AMB NUMEROS DE LÍNIA (OFFSET)
     * Volem fer: while(cnt < b) { res = res * a; cnt++ }
     * * Línia N   : IF cnt GEI b GOTO (N + 4)  <-- Condició sortida
     * Línia N+1 : res := res MUL a           <-- Cos
     * Línia N+2 : cnt := cnt ADDI 1          <-- Cos
     * Línia N+3 : GOTO N                     <-- Tornar inici
     * Línia N+4 : (Continuació del programa)
     */

    int start_label = gen_get_label();  // Obtenim la línia actual (N)

    // Salt de sortida: Si cnt >= exponent, saltem 4 instruccions endavant
    // El destí serà start_label + 4
    emit_if_goto("GEI", t_cnt, b.addr, start_label + 4);

    // Cos: Multiplicar
    //    char op_mul[10] = res.type == TYPE_FLOAT ? "MULF" : "MULI";
    //   emit_binary(op_mul, t_res, t_res, a.addr);

    // Cos: Increment
    emit_binary("ADDI", t_cnt, t_cnt, "1");

    // Tornar a inici
    // emit_goto(start_label); // [cite: 196] Salt incondicional a número de
    // línia

    // Alliberem
    gen_release_temp(a.addr);
    gen_release_temp(b.addr);
    gen_release_temp(t_cnt);  // El comptador ja no el volem

    res.is_literal = 0;
    res.addr = t_res;
    return res;
}