%{
#include <stdio.h>
#include <string.h>
#include "data_types.h"
#include "globals.h"
#include "../src/utils/log.h"
#include "../src/symtab/symtab.h"
#include "../src/utils/error.h"
#include "../src/utils/report.h"
#include "../src/semantic/declarator_list.h"
#include "../src/semantic/handler.h"
#include "../src/semantic/expression/expression.h"

extern int yylex();

%}

%code requires {
  #include "data_types.h"
  #include "../src/semantic/declarator_list.h"
}

%union {
    char *text;
    DATA_TYPE data_type;
    DeclaratorListNode *decl_list;

    int int_val;
    float float_val;

    ExprResult expr_val;
}
%locations
/* Type Tokens */
%token <data_type> TK_INT TK_FLOAT

/* IDs Tokens */
%token <text> TK_ID

%token <int_val> TK_INT_LIT 
%token <float_val> TK_FLOAT_LIT

/* Puntuation and Operation Tokens  */
%token TK_ASSIGN TK_COMMA TK_NEWLINE TK_SEMICOLON TK_DOT TK_EOF
%token TK_PLUS TK_MINUS TK_MULT TK_DIV TK_MOD TK_POW
%token TK_LPAREN TK_RPAREN TK_LBRACKET TK_RBRACKET
%token TK_REPEAT TK_DO TK_DONE

%type <data_type> primitive_type
%type <expr_val>  arith_expr arith_term unary_expr arith_factor primary literal
%type <decl_list> declarator_list declarator
//%type <int_val> repeat_sentence

%define parse.error detailed

%%

program:
    optional_newlines item_list optional_newlines opt_eof
;

item_list:
    program_item
    | item_list optional_newlines program_item
    | item_list error end_of_stmt
      {
        log_production("Recovered from item list error, skipping item.");
        yyerrok;
      }
;

program_item:
    simple_declaration
    | sentence
;

optional_newlines:
    /* void */
    | optional_newlines TK_NEWLINE
;

end_of_stmt:
    TK_NEWLINE
    | TK_EOF 
;

opt_eof:
      /* void */
    | TK_EOF
;


simple_declaration: 
    primitive_type declarator_list end_of_stmt { handle_simple_declaration($1, $2); }
    | primitive_type declarator_list error
    {
        log_production("Recovered from missing \\n");
        declarator_list_free($2);
        yyerrok;
    }
    | primitive_type error end_of_stmt
    {
        log_production("Recovered from identifier list error");
        yyerrok;
    }
;

primitive_type:
    TK_INT      { $$ = TYPE_INT; log_production("Type: int"); }
    | TK_FLOAT  { $$ = TYPE_FLOAT; log_production("Type: float"); }
;

declarator_list:
    declarator { $$ = $1; }
    | declarator_list TK_COMMA declarator { $$ = declarator_list_prepend($1, $3); }
;

declarator:
    TK_ID { $$ = create_declarator($1, 0, 0); }
    | TK_ID TK_LBRACKET TK_INT_LIT TK_RBRACKET { $$ = create_declarator($1, 1, $3); }
;

sentence:
    assignment_sentence end_of_stmt
    | expression_sentence end_of_stmt
    //| repeat_sentence
;

/*
TODO: Com es faria per permetre declaracions de variables dins dun bucle, sense que peti 
repeat_sentence:
    TK_REPEAT arith_expr 
    {
        $<int_val>$ = gen_get_label(); 
    }
    TK_DO  item_list TK_DONE
    {
        int start_label = $3; // L'etiqueta que hem guardat a la posició 3
        char *counter = $2.addr;       // La variable on tenim el número de repeticions

        emit_binary("SUBI", counter, counter, "1");

        emit_if_goto("GEI", counter, "1", start_label);
        
        log_production("Repeat loop generated (Start label: %d)", start_label);
    }
;*/

assignment_sentence:
    TK_ID TK_ASSIGN arith_expr { handle_assigment_sentence($1, $3); }
;

expression_sentence:
    arith_expr 
    {
        report_expression(current_location(), $1);
    }
;

/* Level 5: Sum, Sub, Unary +/- */
arith_expr:
    arith_expr TK_PLUS unary_expr    { $$ = eval_add($1, $3); }
  | arith_expr TK_MINUS unary_expr   { $$ = eval_sub($1, $3); }
  | unary_expr                       { $$ = $1; }
;

/* Level 5.5: Unary */
unary_expr:
    TK_PLUS unary_expr  { $$ = eval_unary_plus($2); }
  | TK_MINUS unary_expr { $$ = eval_unary_minus($2); }
  | arith_term          { $$ = $1; }
;

/* Level 6: *, /, % */
arith_term:
    arith_term TK_MULT arith_factor { $$ = eval_mul($1, $3); }
  | arith_term TK_DIV arith_factor  { $$ = eval_div($1, $3); }
  | arith_term TK_MOD arith_factor  { $$ = eval_mod($1, $3); }
  | arith_factor                    { $$ = $1; }
;

/* Level 7: Pow (**) */
arith_factor:
    primary TK_POW arith_factor { $$ = eval_pow($1, $3); }
    | primary { $$ = $1; }
;

primary:
    TK_LPAREN arith_expr TK_RPAREN { $$ = $2; log_production("Expression: Parenthesized"); }
    | literal { $$ = $1; }
    | TK_ID { $$ = handle_primary_id($1); }
    | TK_ID TK_LBRACKET arith_expr TK_RBRACKET 
    {
        log_production("Parsed Array Access: %s[...]", $1);
        $$.type = TYPE_VOID; 
        $$.addr = NULL;
    }
;

literal:
    TK_INT_LIT
    {
        $$.type = TYPE_INT;
        $$.value.int_val = $1;
        $$.is_constant = 1;
        $$.addr = malloc(64);
        sprintf($$.addr, "%d", $1);
        log_production("Literal: %s (%d)", TYPE_NAME[$$.type], $1);
    }
    | TK_FLOAT_LIT
    {
        $$.type = TYPE_FLOAT;
        $$.value.float_val = $1;
        $$.is_constant = 1;
        $$.addr = malloc(64);
        sprintf($$.addr, "%g", $1);
        log_production("Literal: %s (%g)", TYPE_NAME[$$.type], $1);
    }
%%