#ifndef HANDLERS_UTILS_H
#define HANDLERS_UTILS_H

#include "data_types.h"

ExprResult calculate_array_offset(sym_value_type* entry, ExprResult index_expr, char* token_id);
sym_value_type* get_symbol_checked(char* token_id);
int check_is_array(sym_value_type* entry, const char* token_id);
int check_is_scalar(sym_value_type* entry, const char* token_id);
int check_is_initialized(sym_value_type* entry, const char* token_id);
int check_types_match(DATA_TYPE t1, DATA_TYPE t2);

#endif