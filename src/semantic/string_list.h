#ifndef STRING_LIST_H
#define STRING_LIST_H

typedef struct StringListNode {
    char *id;
    struct StringListNode *next;
} StringListNode;

StringListNode* string_list_append(StringListNode *list, char *id);
int string_list_check_duplicates(const StringListNode *list);
void string_list_free(StringListNode *list);

#endif
