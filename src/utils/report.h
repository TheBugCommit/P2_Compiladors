#ifndef REPORT_H
#define REPORT_H

#include "data_types.h"
#include "globals.h"

void report_assignation(SourceLocation loc, const char* token_id, ExprResult e);
void report_expression(SourceLocation loc, ExprResult e);

#endif