#ifndef DATA_TYPES_H
#define DATA_TYPES_H

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_VOID
} DATA_TYPE;

typedef union {
    long long int int_val;
    double float_val;
} VALUE_UNION;

typedef struct {
    DATA_TYPE type;
    VALUE_UNION value;
} ExprResult;

typedef struct {
    DATA_TYPE type;
    char is_initialized;
    VALUE_UNION value;
} sym_value_type;

#endif