#include "string_list.h"
#include <stdlib.h>
#include <string.h>

StringListNode* string_list_append(StringListNode *list, char *id) {
    StringListNode *node = malloc(sizeof(StringListNode));
    node->id = id;
    node->next = list;
    return node;
}

void string_list_free(StringListNode *list) {
    while (list) {
        StringListNode *tmp = list;
        list = list->next;
        free(tmp->id);
        free(tmp);
    }
}
int string_list_check_duplicates(const StringListNode *list) {
    const StringListNode *outer = list;
    while (outer != NULL) {
        const StringListNode *inner = outer->next;
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