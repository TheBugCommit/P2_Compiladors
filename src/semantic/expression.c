#include "expression.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/error.h"
#include "../utils/log.h"
#include "globals.h"
#include "parser.h"

int is_numeric(ExprResult* e) {
    if (e->type != TYPE_INT && e->type != TYPE_FLOAT) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Type error: expected numeric type (%s or %s)",
                              TYPE_NAME[TYPE_INT], TYPE_NAME[TYPE_FLOAT]);
        return 0;
    }

    return 1;
}

int is_int(ExprResult* e) {
    if (e->type != TYPE_INT) {
        REPORT_SEMANTIC_ERROR(current_location(), "Type error: expected %s type",
                              TYPE_NAME[TYPE_INT]);
        return 0;
    }

    return 1;
}

ExprResult eval_add(ExprResult a, ExprResult b) {
    ExprResult res;
    log_production("Evaluating add");

    if (!is_numeric(&a) && !is_numeric(&b)) {
        res.type = TYPE_VOID;
        return res;
    }
    
    if (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) {
        res.type = TYPE_FLOAT;
        double val_a = (a.type == TYPE_FLOAT) ? a.value.float_val
                                              : (double)a.value.int_val;
        double val_b = (b.type == TYPE_FLOAT) ? b.value.float_val
                                              : (double)b.value.int_val;
        res.value.float_val = val_a + val_b;
    } else if (a.type == TYPE_INT && b.type == TYPE_INT) {
        res.type = TYPE_INT;
        res.value.int_val = a.value.int_val + b.value.int_val;
    } else {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Type error: invalid operands for '+' operator");
        res.type = TYPE_VOID;
    }
    return res;
}

ExprResult eval_sub(ExprResult a, ExprResult b) {
    log_production("Evaluating sub");
    ExprResult res;
    if (!(is_numeric(&a) && is_numeric(&b))) {
        res.type = TYPE_VOID;
        return res;
    }

    if (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) {
        res.type = TYPE_FLOAT;
        double val_a = (a.type == TYPE_FLOAT) ? a.value.float_val
                                              : (double)a.value.int_val;
        double val_b = (b.type == TYPE_FLOAT) ? b.value.float_val
                                              : (double)b.value.int_val;
        res.value.float_val = val_a - val_b;
    } else {
        res.type = TYPE_INT;
        res.value.int_val = a.value.int_val - b.value.int_val;
    }
    return res;
}

ExprResult eval_mul(ExprResult a, ExprResult b) {
    log_production("Evaluating mul");
    ExprResult res;
    if (!(is_numeric(&a) && is_numeric(&b))) {
        res.type = TYPE_VOID;
        return res;
    }

    if (a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) {
        res.type = TYPE_FLOAT;
        double val_a = (a.type == TYPE_FLOAT) ? a.value.float_val
                                              : (double)a.value.int_val;
        double val_b = (b.type == TYPE_FLOAT) ? b.value.float_val
                                              : (double)b.value.int_val;
        res.value.float_val = val_a * val_b;
    } else {
        res.type = TYPE_INT;
        res.value.int_val = a.value.int_val * b.value.int_val;
    }
    return res;
}

ExprResult eval_div(ExprResult a, ExprResult b) {
    log_production("Evaluating div");
    ExprResult res;
    if (!(is_numeric(&a) && is_numeric(&b))) {
        res.type = TYPE_VOID;
        return res;
    }

    if (a.type == TYPE_INT && b.type == TYPE_INT) {
        res.type = TYPE_INT;

        if (b.value.int_val == 0) {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Runtime error: Division by zero");
            res.type = TYPE_VOID;
        } else {
            res.value.int_val = a.value.int_val / b.value.int_val;
        }
    } else {
        res.type = TYPE_FLOAT;

        double val_a = (a.type == TYPE_FLOAT) ? a.value.float_val
                                              : (double)a.value.int_val;
        double val_b = (b.type == TYPE_FLOAT) ? b.value.float_val
                                              : (double)b.value.int_val;

        if (val_b == 0.0) {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Runtime error: Division by zero");
            res.type = TYPE_VOID;
        } else {
            res.value.float_val = val_a / val_b;
        }
    }

    return res;
}

ExprResult eval_mod(ExprResult a, ExprResult b) {
    log_production("Evaluating mod");
    ExprResult res;
    if (!(is_numeric(&a) && is_numeric(&b))) {
        res.type = TYPE_VOID;
        return res;
    }

    if (a.type == TYPE_INT && b.type == TYPE_INT) {
        res.type = TYPE_INT;
        if (b.value.int_val == 0) {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Runtime error: Modulo by zero");
            res.type = TYPE_VOID;
        } else {
            res.value.int_val = a.value.int_val % b.value.int_val;
        }
    } else {
        double val_a = (a.type == TYPE_FLOAT) ? a.value.float_val
                                              : (double)a.value.int_val;
        double val_b = (b.type == TYPE_FLOAT) ? b.value.float_val
                                              : (double)b.value.int_val;

        res.type = TYPE_FLOAT;
        if (val_b == 0.0) {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Runtime error: Modulo by zero");
            res.type = TYPE_VOID;
        } else {
            res.value.float_val = fmod(val_a, val_b);
        }
    }

    return res;
}

ExprResult eval_pow(ExprResult a, ExprResult b) {
    log_production("Evaluating pow");
    ExprResult res;
    if (!(is_numeric(&a) && is_numeric(&b))) {
        res.type = TYPE_VOID;
        return res;
    }

    if (a.type == TYPE_INT && b.type == TYPE_INT) {
        long long int base = a.value.int_val;
        long long int exp = b.value.int_val;

        long long int r;
        if (exp < 0)
            r = (long long int)pow((double)base, (double)exp);
        else {
            r = 1;
            for (long long int i = 0; i < exp; i++) {
                r *= base;
            }
        }

        res.type = TYPE_INT;
        res.value.int_val = r;
    } else {
        double r = pow((a.type == TYPE_FLOAT) ? a.value.float_val
                                              : (double)a.value.int_val,
                       (b.type == TYPE_FLOAT) ? b.value.float_val
                                              : (double)b.value.int_val);

        res.type = TYPE_FLOAT;
        res.value.float_val = r;
    }

    return res;
}

ExprResult eval_unary_minus(ExprResult a) {
    log_production("Evaluating unary minus");
    ExprResult res = a;
    if (!is_numeric(&a)) {
        res.type = TYPE_VOID;
        return res;
    }

    if (a.type == TYPE_FLOAT) {
        res.value.float_val = -a.value.float_val;
    } else {
        res.value.int_val = -a.value.int_val;
    }

    return res;
}

ExprResult eval_unary_plus(ExprResult a) {
    log_production("Evaluating unary plus");
    ExprResult res = a;
    if (!is_numeric(&a)) {
        res.type = TYPE_VOID;
        return res;
    }

    return res;
}
