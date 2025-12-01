%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include "parser.h"
#include "data_types.h"
#include "../src/utils/log.h"
#include "../src/utils/error.h"

extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yycolumn = 0;
int lineno = 0;
int emitted_final_eof = 0;

#define YY_USER_ACTION { \
    int i; \
    yylloc.first_line = lineno + 1; \
    yylloc.first_column = yycolumn - yyleng > 0 ? yycolumn - yyleng : yycolumn; \
    for (i = 0; i < yyleng; ++i) { \
        if (yytext[i] == '\n') { \
            lineno++; \
            yycolumn = 0; \
        } else { \
            yycolumn++; \
        } \
    } \
    yylloc.last_line = lineno; \
    yylloc.last_column = yycolumn; \
}

%}

%option nounput
%option noinput
%option noyywrap

/* States */
%x MULTI_COMMENT

ID              [a-zA-Z_][a-zA-Z0-9_]*
WHITESPACE      [ \t]
RETURN          [\n]
COMMENT         (\/\/|#)
DIGIT           [0-9]
EXPONENT        ([eE][+-]?{DIGIT}+)
FLOAT           (({DIGIT}+\.{DIGIT}+({EXPONENT})?)|(\.{DIGIT}+({EXPONENT})?)|({DIGIT}+{EXPONENT})|({DIGIT}+\.))
INT             {DIGIT}+

%%

"/*" { 
    BEGIN(MULTI_COMMENT);
    SourceLocation loc = current_location();
    log_production("Multiline comment start on row: %d col: %d", loc.line, loc.column);
}
<MULTI_COMMENT>[^*]+ { log_production("Multicomment skipping characters"); }
<MULTI_COMMENT>\*+\/ {
    BEGIN(INITIAL);
    SourceLocation loc = current_location();
    log_production("Multiline comment closed on row: %d col: %d", loc.line, loc.column);
}
<MULTI_COMMENT>.|\n  { }
<MULTI_COMMENT><<EOF>> {
    SourceLocation loc = current_location();
    REPORT_LEXICAL_ERROR(loc, "Unclosed multi-line comment");
    BEGIN(INITIAL);
}

{COMMENT}[^\n]*     { log_production("Skipped comment until EOL"); }
{COMMENT}[^\n]*\n   { log_production("Skipped full comment line"); return TK_NEWLINE; }

%{ /* Types */ %}

"int"       { yylval.data_type = TYPE_INT; return TK_INT; }
"float"     { yylval.data_type = TYPE_FLOAT; return TK_FLOAT; }

%{ /* Operators and expression keywords */ %}

":="        { return TK_ASSIGN; }
"+"         { return TK_PLUS; }
"-"         { return TK_MINUS; }
"*"         { return TK_MULT; }
"/"         { return TK_DIV; }
"%"         { return TK_MOD; }
"**"        { return TK_POW; }
"("         { return TK_LPAREN; }
")"         { return TK_RPAREN; }
"["         { return TK_LBRACKET; }
"]"         { return TK_RBRACKET; }

%{ /* Stroctures */ %}

"repeat"    { return TK_REPEAT; }
"do"        { return TK_DO; }
"done"      { return TK_DONE; }

%{ /* Literals */ %}

{FLOAT}     { 
    errno = 0;
    char *endptr;
    double val = strtod(yytext, &endptr);

    if (endptr == yytext) {
        REPORT_LEXICAL_ERROR(current_location(), "Invalid float literal: %s", yytext);
    }
    else if (errno == ERANGE) {
        REPORT_LEXICAL_ERROR(current_location(), "Float literal out of range: %s", yytext);
    }
    else {
        yylval.float_val = val;
        return TK_FLOAT_LIT;
    }
}

{INT}       { 
    errno = 0;
    char *endptr;
    long long val = strtoll(yytext, &endptr, 10);
    if (endptr == yytext) {
        REPORT_LEXICAL_ERROR(current_location(), "Invalid integer literal: %s", yytext);
    }
    else if (errno == ERANGE || val > LLONG_MAX || val < LLONG_MIN) {
        REPORT_LEXICAL_ERROR(current_location(), "Integer literal out of range: %s", yytext);
    }
    else {
        yylval.int_val = val;
        return TK_INT_LIT;
    }
}

%{ /* Identifiers */ %}

{ID} { 
    yylval.text = strdup(yytext); 
    return TK_ID; 
}

%{ /* Puntuation */ %}
","         { return TK_COMMA; }
";"         { return TK_SEMICOLON; }
"."         { return TK_DOT; }

{WHITESPACE}+      { }
{RETURN}+          { return TK_NEWLINE; }

<<EOF>>     { 
    if (!emitted_final_eof) {
        emitted_final_eof = 1;
        return TK_EOF;
    } else {
        return 0;
    }
}

. { 
    SourceLocation loc = current_location();
    REPORT_LEXICAL_ERROR(loc, "Invalid character: '%s'", yytext);
}

%%