#ifndef DATA_TYPES_H
#define DATA_TYPES_H

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_VOID
} DATA_TYPE;

typedef union {
    int int_val;
    float float_val;
} VALUE_UNION;

typedef struct {
    DATA_TYPE type;
    VALUE_UNION value;
    char *addr;
} ExprResult;

typedef struct {
    DATA_TYPE type;
    char is_initialized;
    VALUE_UNION value;
    int is_array;
    int size;
    char *addr;
} sym_value_type;

#endif