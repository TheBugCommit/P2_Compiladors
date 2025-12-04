#include "expression_priv.h"
#include <stdlib.h>
#include <stdio.h>
#include "../../codegen/codegen.h"
#include "../../codegen/gen_temps.h"
#include "../../utils/log.h"

ExprResult generate_arithmetic(ExprResult a, ExprResult b, char* op_int,
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

ExprResult generate_pow_code(ExprResult a, ExprResult b) {
    ExprResult res = {TYPE_VOID};

    if (b.is_constant && b.value.int_val >= 0 && b.value.int_val <= 4) {
        return pow_optimization(a, b);
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

ExprResult generate_arithmetic_code(ExprResult a, ExprResult b, ArithOp op, 
                                    char* op_int, char* op_float) {
    
    log_production("Generating code");

    if (op == OP_POW) {
        return generate_pow_code(a, b);
    } else {
        return generate_arithmetic(a, b, op_int, op_float);
    }
}