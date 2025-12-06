#include <stdio.h>
#include <stdlib.h>

#include "../../utils/error.h"
#include "../expression/expression_utils.h"
#include "handlers.h"

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
            free(token_id);
            free(entry);
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