#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "codegen.h"

static Quad* quads = NULL;
static int n_quads = 0;
static int next_quad;

static FILE* out = NULL;

static void free_quads_memory() {
    if (quads) {
        for (int i = 0; i < next_quad; i++) {
            if (quads[i].op) free(quads[i].op);
            if (quads[i].res) free(quads[i].res);
            if (quads[i].arg1) free(quads[i].arg1);
            if (quads[i].arg2) free(quads[i].arg2);
        }
        free(quads);
        quads = NULL;
    }
    n_quads = 0;
    next_quad = 0;
}

void code_init(FILE* output_file) {
    out = output_file ? output_file : stdout;
    free_quads_memory();
}

static void emit(const char* op, const char* res, const char* arg1,
                 const char* arg2) {
    if (next_quad >= n_quads) {
        int new_capacity = (n_quads == 0) ? 128 : n_quads * 2;
        Quad* temp = realloc(quads, new_capacity * sizeof(Quad));
        if (!temp) {
            fprintf(stderr, "FATAL ERROR: Out of memory reallocating quads.\n");
            exit(1);
        }

        quads = temp;
        n_quads = new_capacity;
    }

    Quad* q = &quads[next_quad];
    q->op = op ? strdup(op) : NULL;
    q->res = res ? strdup(res) : NULL;
    q->arg1 = arg1 ? strdup(arg1) : NULL;
    q->arg2 = arg2 ? strdup(arg2) : NULL;

    next_quad++;
}

int gen_get_label() { return next_quad + 1; }

void emit_binary(const char* op, const char* dest, const char* arg1,
                 const char* arg2) {
    emit(op, dest, arg1, arg2);
}

void emit_unary(const char* op, const char* dest, const char* arg1) {
    emit(op, dest, arg1, NULL);
}

void emit_assign(const char* dest, const char* src) {
    emit(":=", dest, src, NULL);
}

void emit_goto(int label) {
    char label_str[20];
    sprintf(label_str, "%d", label);
    emit("GOTO", NULL, label_str, NULL);
}

void emit_if_goto(const char* op, const char* arg1, const char* arg2, int label) {
    char label_str[20];
    sprintf(label_str, "%d", label);

    char full_op[32];
    sprintf(full_op, "IF %s", op);

    emit(full_op, label_str, arg1, arg2);
}

void emit_io(const char* param, const char* prod, const int args) {
    emit("PARAM", NULL, param, NULL);

    char call_arg[64];
    sprintf(call_arg, "%s, %d", prod, args);
    emit("CALL", NULL, call_arg, NULL);
}

void emit_array_load(const char* dest, const char* array_name,
                     const char* offset) {
    char ref[128];
    sprintf(ref, "%s[%s]", array_name, offset);
    emit(":=", dest, ref, NULL);
}

void emit_array_store(const char* array_name, const char* offset,
                      const char* value) {
    char ref[128];
    sprintf(ref, "%s[%s]", array_name, offset);
    emit(":=", ref, value, NULL);
}

void emit_halt() { emit("HALT", NULL, NULL, NULL); }

void print_quads() {
    if (!out) return;

    for (int i = 0; i < next_quad; i++) {
        Quad* q = &quads[i];

        fprintf(out, "%d:\t", i + 1);

        if (q->op == NULL) {
            fprintf(out, "\n");
            continue;
        }

        if (strcmp(q->op, ":=") == 0) {
            fprintf(out, "%s := %s", q->res, q->arg1);
        } else if (strcmp(q->op, "GOTO") == 0) {
            fprintf(out, "GOTO %s", q->arg1);
        } else if (strncmp(q->op, "IF", 2) == 0) {
            char* rel = q->op + 3;
            fprintf(out, "IF %s %s %s GOTO %s", q->arg1, rel, q->arg2, q->res);
        } else if (strcmp(q->op, "PARAM") == 0) {
            fprintf(out, "PARAM %s", q->arg1);
        } else if (strcmp(q->op, "CALL") == 0) {
            fprintf(out, "CALL %s", q->arg1);
        } else if (strcmp(q->op, "HALT") == 0) {
            fprintf(out, "HALT");
        } else {
            if (q->arg2) {
                fprintf(out, "%s := %s %s %s", q->res, q->arg1, q->op, q->arg2);
            } else {
                fprintf(out, "%s := %s %s", q->res, q->op, q->arg1);
            }
        }

        fprintf(out, "\n");
    }
}

void code_close() {
    if (out && out != stdout) {
        fclose(out);
        out = NULL;
    }
    free_quads_memory();
}