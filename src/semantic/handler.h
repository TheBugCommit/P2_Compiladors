#ifndef HANDLER_H
#define HANDLER_H

#include "data_types.h"
#include "./declarator_list.h"

void handle_simple_declaration(DATA_TYPE declared_type, DeclaratorListNode *list);
void handle_assigment_sentence(char * token_id, ExprResult expression);
ExprResult handle_primary_id(char* token_id);

#endif
