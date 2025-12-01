#ifndef DECLARATOR_LIST_H
#define DECLARATOR_LIST_H

typedef struct DeclaratorListNode {
    char* id;
    int is_array;
    int array_size;
    struct DeclaratorListNode* next;
} DeclaratorListNode;

DeclaratorListNode* create_declarator(char* id, int is_array, int size);
DeclaratorListNode* declarator_list_prepend(DeclaratorListNode* list,
                                            DeclaratorListNode* new_node);
int declarator_list_check_duplicates(const DeclaratorListNode* list);
void declarator_list_free(DeclaratorListNode* list);

#endif
