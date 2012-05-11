%{
#include "ast.h"
#include "parser.h"
#define SAVE_STRING yylval.string = strndup(yytext, yyleng)
#define SAVE_NUMBER yylval.number = atof(yytext)
#define TOKEN(t) (yylval.token = t)
%}

%option noyywrap

%%

[ \t\n]                 ;
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_STRING; return TIDENTIFIER;
[0-9]*                  SAVE_NUMBER; return TNUMBER;

%%