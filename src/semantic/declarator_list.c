#include "declarator_list.h"

#include <stdlib.h>
#include <string.h>

#include "../utils/error.h"

int declarator_list_check_duplicates(const DeclaratorListNode* list) {
    const DeclaratorListNode* outer = list;
    while (outer != NULL) {
        const DeclaratorListNode* inner = outer->next;
        while (inner != NULL) {
            if (strcmp(outer->id, inner->id) == 0) {
                return 1;
            }
            inner = inner->next;
        }
        outer = outer->next;
    }
    return 0;
}

DeclaratorListNode* create_declarator(char* id, int is_array, int size) {
    if (is_array && size <= 0) {
        REPORT_SEMANTIC_ERROR(current_location(),
                              "Array size can't not be 0 or negative")
        return NULL;
    }

    DeclaratorListNode* node = malloc(sizeof(DeclaratorListNode));
    node->id = id;
    node->is_array = is_array;
    node->array_size = size;
    node->next = NULL;
    return node;
}

DeclaratorListNode* declarator_list_prepend(DeclaratorListNode* list,
                                            DeclaratorListNode* new_node) {
    if (new_node == NULL) return list;
    new_node->next = list;
    return new_node;
}

void declarator_list_free(DeclaratorListNode* list) {
    while (list) {
        DeclaratorListNode* tmp = list;
        list = list->next;
        if (tmp->id) free(tmp->id);
        free(tmp);
    }
}