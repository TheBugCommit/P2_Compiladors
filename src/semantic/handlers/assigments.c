#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../codegen/codegen.h"
#include "../../codegen/gen_temps.h"
#include "../../symtab/symtab.h"
#include "../../utils/error.h"
#include "../../utils/log.h"
#include "../../utils/report.h"
#include "../expression/expression.h"
#include "../expression/expression_utils.h"
#include "globals.h"
#include "handlers.h"

void handle_assigment_sentence(char* token_id, ExprResult expression) {
    sym_value_type* entry = malloc(sizeof(sym_value_type));
    int res = sym_lookup(token_id, entry);

    if (res == SYMTAB_NOT_FOUND) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Assignment to undeclared identifier '%s'",
                              token_id);
        free(token_id);
        free(entry);
        return;
    }

    if (entry->is_array) {
        REPORT_SEMANTIC_ERROR(
            current_location(),
            "Cannot assign to array '%s' directly. Use index.", token_id);
        free(token_id);
        free(entry);
        return;
    }

    expression = ensure_type(expression, entry->type);
    emit_assign(token_id, expression.addr);

    log_production("Assignment: '%s' := %s", token_id, expression.addr);

    entry->is_initialized = 1;

    /* TODO: Constant propagation (vigilar amb repeat)
    if (expression.is_constant) {
         entry->is_constant = 1;

         if (entry->type == TYPE_INT) {
             entry->value.int_val = (expression.type == TYPE_FLOAT)
                                        ? (int)expression.value.float_val
                                        : expression.value.int_val;
         } else {
             entry->value.float_val = (expression.type == TYPE_INT)
                                          ? (float)expression.value.int_val
                                          : expression.value.float_val;
         }
         log_production("Constant Propagation: Variable '%s' is now constant
    %s", token_id, expression.addr); } else { entry->is_constant = 0;
     }*/

    entry->is_constant = 0;
    sym_enter(token_id, entry);

    gen_release_temp(expression.addr);
    if (expression.addr) free(expression.addr);
    free(token_id);
    free(entry);
}

void handle_array_assignment(char* token_id, ExprResult index_expr,
                             ExprResult value_expr) {
    sym_value_type* entry = malloc(sizeof(sym_value_type));
    int lookup_res = sym_lookup(token_id, entry);

    if (lookup_res == SYMTAB_NOT_FOUND) {
        REPORT_SEMANTIC_ERROR(current_location(), "Undeclared array '%s'",
                              token_id);
        free(token_id);
        free(entry);
        return;
    }
    if (!entry->is_array) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Variable '%s' is not an array", token_id);
        free(token_id);
        free(entry);
        return;
    }

    ExprResult final_offset =
        calculate_array_offset(entry, index_expr, token_id);
    if (final_offset.type == TYPE_VOID) {
        free(token_id);
        free(entry);
        return;
    }

    value_expr = ensure_type(value_expr, entry->type);
    char* final_val_addr = value_expr.addr;

    emit_array_store(token_id, final_offset.addr, final_val_addr);

    log_production("Array Store: %s[...]", token_id);

    if (final_offset.addr) {
        gen_release_temp(final_offset.addr);
        free(final_offset.addr);
    }

    gen_release_temp(value_expr.addr);
    if (value_expr.addr) free(value_expr.addr);

    free(token_id);
    free(entry);
}