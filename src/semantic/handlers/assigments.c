#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../codegen/codegen.h"
#include "../../codegen/gen_temps.h"
#include "../../symtab/symtab.h"
#include "../../utils/error.h"
#include "../../utils/log.h"
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
        if (expression.addr) free(expression.addr);
        return;
    }

    if (entry->type != expression.type) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Type mismatch in assigment.");
        free(token_id);
        free(entry);
        if (expression.addr) free(expression.addr);
        return;
    }

    if (entry->is_array) {
        if (!expression.is_array) {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Cannot assign scalar to array '%s'. "
                                  "Expecting array of same size.",
                                  token_id);
            free(token_id);
            free(entry);
            if (expression.addr) free(expression.addr);
            return;
        }

        if (entry->size != expression.size) {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Size mismatch: Dest[%d] vs Src[%d]",
                                  entry->size, expression.size);
            free(token_id);
            free(entry);
            if (expression.addr) free(expression.addr);
            return;
        }

        log_production("Array Block Copy: %s := %s", token_id, expression.addr);
        emit_assign(token_id, expression.addr);

        entry->is_initialized = 1;
        sym_enter(token_id, entry);

        free(token_id);
        free(entry);
        if (expression.addr) free(expression.addr);
        return;
    }

    if (expression.is_array) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Cannot assign array to scalar '%s'.", token_id);
        free(token_id);
        free(entry);
        if (expression.addr) free(expression.addr);
        return;
    }

    emit_assign(token_id, expression.addr);

    log_production("Assignment: '%s' := %s", token_id, expression.addr);

    entry->is_initialized = 1;
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
        if (value_expr.addr) free(value_expr.addr);
        return;
    }
    if (!entry->is_array) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Variable '%s' is not an array", token_id);
        free(token_id);
        free(entry);
        if (value_expr.addr) free(value_expr.addr);
        return;
    }

    if (entry->type != value_expr.type) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Type mismatch in assigment.");
        free(token_id);
        free(entry);
        if (value_expr.addr) free(value_expr.addr);
        return;
    }

    ExprResult final_offset =
        calculate_array_offset(entry, index_expr, token_id);
    if (final_offset.type == TYPE_VOID) {
        free(token_id);
        free(entry);
        return;
    }

    entry->is_initialized = 1;
    sym_enter(token_id, entry);

    emit_array_store(token_id, final_offset.addr, value_expr.addr);

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