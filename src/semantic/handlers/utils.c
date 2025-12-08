#include <stdio.h>
#include <stdlib.h>

#include "../../utils/error.h"
#include "../expression/expression_utils.h"
#include "../../symtab/symtab.h"
#include "handlers.h"
#include "handlers_utils.h"

ExprResult calculate_array_offset(sym_value_type* entry, ExprResult index_expr,
                                  char* token_id) {
    ExprResult res = {TYPE_VOID};

    if (index_expr.type != TYPE_INT) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Array index must be an integer");
        return res;
    }

    if (index_expr.is_constant) {
        int idx = index_expr.value.int_val;
        if (idx < 0 || idx >= entry->size) {
            REPORT_SEMANTIC_ERROR(
                current_location(),
                "Array index out of bounds. Accessing '%s[%d]' but size is %d.",
                token_id, idx, entry->size);
            return res;
        }
    }

    ExprResult size_lit = make_literal_result(TYPE_INT, 4);
    ExprResult rel_offset = eval_mul(index_expr, size_lit);

    ExprResult base_lit = make_literal_result(TYPE_INT, entry->base_addr);
    ExprResult final_offset =
        eval_add(base_lit, rel_offset);  // TODO: Sobra o no?

    return final_offset;
}

sym_value_type* get_symbol_checked(char* token_id) {
    sym_value_type* entry = malloc(sizeof(sym_value_type));
    int res = sym_lookup(token_id, entry);

    if (res == SYMTAB_NOT_FOUND) {
        REPORT_SEMANTIC_ERROR(current_location(), "Undeclared identifier '%s'",
                              token_id);
        free(entry);
        return NULL;
    }

    return entry;
}

int check_is_array(sym_value_type* entry, const char* token_id) {
    if (!entry->is_array) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Variable '%s' is not an array. Array expected.",
                              token_id);
        return 0;
    }
    return 1;
}

int check_is_scalar(sym_value_type* entry, const char* token_id) {
    if (entry->is_array) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Variable '%s' is an array. Scalar expected.",
                              token_id);
        return 0;
    }
    return 1;
}

int check_is_initialized(sym_value_type* entry, const char* token_id) {
    if (!entry->is_initialized) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Identifier '%s' used before initialization",
                              token_id);
        return 0;
    }
    return 1;
}

int check_types_match(DATA_TYPE t1, DATA_TYPE t2) {
    if (t1 != t2) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Type mismatch. Expected %s, got %s.",
                              TYPE_NAME[t1], TYPE_NAME[t2]);
        return 0;
    }
    return 1;
}