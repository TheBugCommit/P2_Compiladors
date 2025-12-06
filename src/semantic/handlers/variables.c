#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../codegen/codegen.h"
#include "../../codegen/gen_temps.h"
#include "../../symtab/symtab.h"
#include "../../utils/error.h"
#include "../../utils/log.h"
#include "../../utils/report.h"
#include "../expression/expression_utils.h"
#include "globals.h"
#include "handlers.h"

ExprResult handle_primary_id(char* token_id) {
    ExprResult expr;
    sym_value_type* entry = malloc(sizeof(sym_value_type));
    int res = sym_lookup(token_id, entry);
    if (res != SYMTAB_OK) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Identifier '%s' not declared", token_id);
        expr.type = TYPE_VOID;
        expr.addr = NULL;
    } else if (entry->is_initialized == 0) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Identifier '%s' used before initialization",
                              token_id);
        expr.type = TYPE_VOID;
        expr.addr = NULL;
    } else {
        expr.type = entry->type;
        expr.value = entry->value;
        expr.is_array = entry->is_array;
        expr.size = entry->size;
        expr.is_constant = entry->is_constant;
        expr.addr = strdup(entry->addr);

        /*if (entry->is_constant) {
            TODO: Constant propagation (vigilar amb repeat)
            expr.addr = malloc(ADDRESS_SIZE);
            if (entry->type == TYPE_INT)
                sprintf(expr.addr, "%d", entry->value.int_val);
            else
                sprintf(expr.addr, "%g", entry->value.float_val);

            log_production("Constant Propagation: Using value '%s' for id '%s'",
                           expr.addr, token_id);
        } else {
            expr.addr = strdup(entry->addr);
        }*/
        log_production("Expression: Identifier '%s' -> Type: %s", token_id,
                       TYPE_NAME[expr.type]);
    }

    free(token_id);
    free(entry);

    return expr;
}

ExprResult handle_array_access(char* token_id, ExprResult index_expr) {
    ExprResult res = {TYPE_VOID};
    sym_value_type* entry = malloc(sizeof(sym_value_type));
    int lookup_res = sym_lookup(token_id, entry);

    if (lookup_res != SYMTAB_OK) {
        REPORT_SEMANTIC_ERROR(current_location(), "Undeclared array '%s'",
                              token_id);
        free(token_id);
        free(entry);
        if (index_expr.addr) free(index_expr.addr);
        return res;
    }

    if (!entry->is_array) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Variable '%s' is not an array", token_id);
        free(token_id);
        free(entry);
        if (index_expr.addr) free(index_expr.addr);
        return res;
    }

    log_production("Array Access: %s[...] (offset calculation)", token_id);

    ExprResult final_offset =
        calculate_array_offset(entry, index_expr, token_id);
    if (final_offset.type == TYPE_VOID) {
        free(token_id);
        free(entry);
        return res;
    }
    
    res.type = entry->type;
    res.is_constant = 0;
    res.is_array = 0;

    if (is_temporary(final_offset.addr)) {
        res.addr = strdup(final_offset.addr);
    } else {
        res.addr = gen_new_temp();
    }

    emit_array_load(res.addr, entry->addr, final_offset.addr);

    if (final_offset.addr) {
        if (is_temporary(final_offset.addr) &&
            strcmp(res.addr, final_offset.addr) != 0) {
            gen_release_temp(final_offset.addr);
        }
        free(final_offset.addr);
    }

    free(token_id);
    free(entry);

    return res;
}