#include "handler.h"

#include <stdlib.h>
#include <string.h>

#include "../symtab/symtab.h"
#include "../utils/error.h"
#include "../utils/log.h"
#include "./expression.h"
#include "../utils/report.h"
#include "globals.h"

void handle_simple_declaration(DATA_TYPE declared_type, StringListNode* list) {
    StringListNode* current = list;

    while (current != NULL) {
        sym_value_type* new_entry = malloc(sizeof(sym_value_type));
        new_entry->type = declared_type;
        new_entry->is_initialized = 0;

        int result = sym_add(current->id, new_entry);
        if (result == SYMTAB_DUPLICATE)
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Identifier '%s' already declared",
                                  current->id);

        log_production("Declared identifier: '%s' (Type: %s)", current->id,
                       TYPE_NAME[declared_type]);

        current = current->next;
    }

    string_list_free(current);
}

int assigment_sentence(char* token_id, sym_value_type* entry,
                       ExprResult expression) {
    if (entry->type != expression.type) {
        REPORT_SEMANTIC_ERROR(
            current_location(),
            "Type mismatch in assignment to '%s'. Expected %s, got %s",
            token_id, TYPE_NAME[entry->type], TYPE_NAME[expression.type]);
        return 0;
    }

    switch (entry->type) {
        case TYPE_INT:
            entry->value.int_val = expression.value.int_val;
            log_production("Assignment: '%s' = %lld [Type: %s]", token_id,
                           entry->value.int_val, TYPE_NAME[entry->type]);
            break;
        case TYPE_FLOAT:
            entry->value.float_val = expression.value.float_val;
            log_production("Assignment: '%s' = %g [Type: %s]", token_id,
                           entry->value.float_val, TYPE_NAME[entry->type]);
            break;
        default:
            break;
    }

    entry->is_initialized = 1;

    return 1;
}

void handle_assigment_sentence(char* token_id, ExprResult expression) {
    sym_value_type* entry = malloc(sizeof(sym_value_type));
    int res = sym_lookup(token_id, entry);
    if (res == SYMTAB_NOT_FOUND) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Assignment to undeclared identifier '%s'",
                              token_id);
        free(token_id);
        return;
    }

    if (assigment_sentence(token_id, entry, expression)) {
        res = sym_enter(token_id, entry);
        if (res == SYMTAB_DUPLICATE) {
            log_production("Symbtab updated");
        }

        report_assignation(current_location(), token_id, expression);
    }

    free(token_id);
}

ExprResult handle_primary_id(char* token_id) {
    ExprResult expr;
    sym_value_type* entry = malloc(sizeof(sym_value_type));
    int res = sym_lookup(token_id, entry);
    if (res != SYMTAB_OK) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Identifier '%s' not declared", token_id);
        expr.type = TYPE_VOID;
    } else if (entry->is_initialized == 0) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Identifier '%s' used before initialization",
                              token_id);
        expr.type = TYPE_VOID;
    } else {
        expr.type = entry->type;
        expr.value = entry->value;

        log_production("Expression: Identifier '%s'", token_id);
    }

    free(token_id);

    return expr;
}