#include "./report.h"

#include <stdio.h>

#include "globals.h"

void print_value(ExprResult e) {
    switch (e.type) {
        case TYPE_INT:
            printf("%lld", e.value.int_val);
            break;
        case TYPE_FLOAT:
            printf("%g", e.value.float_val);
            break;
        case TYPE_VOID:
            break;
    }
}

void report_expression(SourceLocation loc, ExprResult e) {
    if (e.type == TYPE_VOID) return;

    printf("[%d] ", loc.line);
    print_value(e);
    printf("\n");
}

void report_assignation(SourceLocation loc, const char* token_id,
                        ExprResult e) {
    if (e.type == TYPE_VOID) return;

    printf("[%d] %s := ", loc.line, token_id);
    print_value(e);
    printf("\n");
}

