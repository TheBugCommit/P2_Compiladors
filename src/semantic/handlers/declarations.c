#include "handlers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../symtab/symtab.h"
#include "../../utils/error.h"
#include "../../utils/log.h"
#include "globals.h"


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
        new_entry->base_addr = global_arrays_offset;
        if (current->is_array) {
            global_arrays_offset += current->array_size * VAR_BYTES;
        }

        int result = sym_add(strdup(current->id), new_entry);
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

    declarator_list_free(list);
}