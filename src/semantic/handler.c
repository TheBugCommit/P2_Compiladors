#include "handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../codegen/codegen.h"
#include "../codegen/gen_temps.h"
#include "../symtab/symtab.h"
#include "../utils/error.h"
#include "../utils/log.h"
#include "../utils/report.h"
#include "globals.h"

static char* ensure_assignment_type(DATA_TYPE var_type, ExprResult* expr) {
    if (var_type == expr->type) return expr->addr;

    if (var_type == TYPE_FLOAT && expr->type == TYPE_INT) {
        log_production("Generating runtime conversion I2F");
        char* t_conv = gen_new_temp();
        emit_unary("I2F", t_conv, expr->addr);

        gen_release_temp(expr->addr);
        if (expr->addr) free(expr->addr);

        return t_conv;
    }

    if (var_type == TYPE_INT && expr->type == TYPE_FLOAT) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Warning: Assigning FLOAT to INT variable "
                              "(precision loss ignored in C3A)");
    }

    return expr->addr;
}

void handle_simple_declaration(DATA_TYPE declared_type,
                               DeclaratorListNode* list) {
    DeclaratorListNode* current = list;

    while (current != NULL) {
        sym_value_type* new_entry = malloc(sizeof(sym_value_type));
        new_entry->type = declared_type;
        new_entry->is_initialized = 0;

        new_entry->is_array = current->is_array;
        new_entry->size = current->array_size;
        new_entry->addr = strdup(current->id);

        int result = sym_add(current->id, new_entry);
        if (result == SYMTAB_DUPLICATE) {
            REPORT_SEMANTIC_ERROR(current_location(),
                                  "Identifier '%s' already declared",
                                  current->id);

            free(new_entry);
        } else {
            if (current->is_array) {
                log_production("Declared ARRAY: '%s' [%d] (Type: %s)",
                               current->id, current->array_size,
                               TYPE_NAME[declared_type]);
            } else {
                log_production("Declared VAR: '%s' (Type: %s)", current->id,
                               TYPE_NAME[declared_type]);
            }
        }

        current = current->next;
    }

    declarator_list_free(current);
}

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

    char* final_addr = ensure_assignment_type(entry->type, &expression);
    emit_assign(token_id, final_addr);

    log_production("Assignment: '%s' := %s", token_id, final_addr);

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
        log_production("Constant Propagation: Variable '%s' is now constant %s",
                       token_id, expression.addr);
    } else {
        entry->is_constant = 0;
    }*/
    
    entry->is_constant = 0;
    sym_enter(token_id, entry);

    gen_release_temp(final_addr);
    if (final_addr) free(final_addr);
    free(token_id);
    free(entry);
}

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

        if (entry->is_constant) {
            expr.addr = strdup(entry->addr);

            /* TODO: Constant propagation (vigilar amb repeat)
            expr.addr = malloc(ADDRESS_SIZE);
            if (entry->type == TYPE_INT)
                sprintf(expr.addr, "%d", entry->value.int_val);
            else
                sprintf(expr.addr, "%g", entry->value.float_val);

            log_production("Constant Propagation: Using value '%s' for id '%s'",
                           expr.addr, token_id);*/
        } else {
            expr.addr = strdup(entry->addr);
        }

        log_production("Expression: Identifier '%s' -> Type: %s", token_id,
                       TYPE_NAME[expr.type]);
    }

    free(token_id);
    free(entry);

    return expr;
}