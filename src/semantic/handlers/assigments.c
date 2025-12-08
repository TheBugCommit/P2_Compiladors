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

static void cleanup_assign(char* id, sym_value_type* e, ExprResult* ex) {
    if (id) free(id);
    if (e) free(e);
    if (ex && ex->addr) free(ex->addr);
}

void handle_assigment_sentence(char* token_id, ExprResult expression) {
    sym_value_type* entry = get_symbol_checked(token_id);
    if (!entry) {
        cleanup_assign(token_id, NULL, &expression);
        return;
    }

    if (!check_types_match(entry->type, expression.type)) {
        cleanup_assign(token_id, entry, &expression);
        return;
    }

    if (entry->is_array) {
        if (expression.is_array) {
            if (entry->size != expression.size) {
                REPORT_SEMANTIC_ERROR(current_location(),
                                      "Size mismatch in array copy.");
                cleanup_assign(token_id, entry, &expression);
                return;
            }
            log_production("Array Copy: %s := %s", token_id, expression.addr);
            emit_assign(token_id, expression.addr);
        } else {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Cannot assign scalar to array '%s'.",
                                  token_id);
            cleanup_assign(token_id, entry, &expression);
            return;
        }
    } else {
        if (expression.is_array) {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Cannot assign array to scalar '%s'.",
                                  token_id);
            cleanup_assign(token_id, entry, &expression);
            return;
        }
        log_production("Assignment: '%s' := %s", token_id, expression.addr);
        emit_assign(token_id, expression.addr);
    }

    entry->is_initialized = 1;
    entry->is_constant = 0;
    sym_enter(token_id, entry);

    gen_release_temp(expression.addr);
    cleanup_assign(token_id, entry, &expression);
}

void handle_array_assignment(char* token_id, ExprResult index_expr,
                             ExprResult value_expr) {
    sym_value_type* entry = get_symbol_checked(token_id);
    if (!entry) {
        cleanup_assign(token_id, NULL, &value_expr);
        if (index_expr.addr) free(index_expr.addr);
        return;
    }

    if (!check_is_array(entry, token_id) ||
        !check_types_match(entry->type, value_expr.type)) {
        cleanup_assign(token_id, entry, &value_expr);
        if (index_expr.addr) free(index_expr.addr);
        return;
    }

    ExprResult final_offset =
        calculate_array_offset(entry, index_expr, token_id);
    if (final_offset.type == TYPE_VOID) {
        cleanup_assign(token_id, entry, &value_expr);
        if (index_expr.addr) free(index_expr.addr);
        return;
    }

    emit_array_store(token_id, final_offset.addr, value_expr.addr);
    log_production("Array Store: %s[...]", token_id);

    entry->is_initialized = 1;
    sym_enter(token_id, entry);

    if (final_offset.addr) {
        gen_release_temp(final_offset.addr);
        free(final_offset.addr);
    }

    gen_release_temp(value_expr.addr);
    if (value_expr.addr) free(value_expr.addr);

    free(token_id);
    free(entry);
}