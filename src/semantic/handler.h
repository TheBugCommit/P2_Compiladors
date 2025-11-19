#ifndef HANDLER_H
#define HANDLER_H

#include "data_types.h"
#include "./string_list.h"

void handle_simple_declaration(DATA_TYPE declared_type, StringListNode *list);
void handle_assigment_sentence(char * token_id, ExprResult expression);
ExprResult handle_primary_id(char* token_id);
int assigment_sentence(char *token_id, sym_value_type* entry, ExprResult expression);

#endif
