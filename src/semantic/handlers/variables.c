#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../codegen/codegen.h"
#include "../../codegen/gen_temps.h"
#include "../../symtab/symtab.h"
#include "../../utils/error.h"
#include "../../utils/log.h"
#include "../expression/expression_utils.h"
#include "globals.h"
#include "handlers.h"

ExprResult handle_primary_id(char* token_id) {
    ExprResult expr = {TYPE_VOID};

    sym_value_type* entry = get_symbol_checked(token_id);
    if (!entry) {
        free(token_id);
        return expr;
    }

    if (!check_is_initialized(entry, token_id)) {
        free(entry);
        free(token_id);
        return expr;
    }

    expr.type = entry->type;
    expr.value = entry->value;
    expr.is_array = entry->is_array;
    expr.size = entry->size;
    expr.is_constant = entry->is_constant;
    expr.addr = strdup(entry->addr);

    log_production("Expression: Identifier '%s' -> Type: %s", token_id,
                   TYPE_NAME[expr.type]);

    free(token_id);
    free(entry);

    return expr;
}

ExprResult handle_array_access(char* token_id, ExprResult index_expr) {
    ExprResult res = {TYPE_VOID};

    sym_value_type* entry = get_symbol_checked(token_id);
    if (!entry) {
        free(token_id);
        if (index_expr.addr) free(index_expr.addr);
        return res;
    }

    if (!check_is_array(entry, token_id) ||
        !check_is_initialized(entry, token_id)) {
        free(entry);
        free(token_id);
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

    res.addr = is_temporary(final_offset.addr) ? strdup(final_offset.addr)
                                               : gen_new_temp();
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