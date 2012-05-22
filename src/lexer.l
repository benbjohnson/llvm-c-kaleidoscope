%{
#include "ast.h"
#include "parser.h"
#define SAVE_STRING yylval.string = strndup(yytext, yyleng)
#define SAVE_NUMBER yylval.number = atof(yytext)
#define TOKEN(t) (yylval.token = t)
%}

%option noyywrap

%%

"def"                   return TOKEN(TDEF);
"extern"                return TOKEN(TEXTERN);
"if"                    return TOKEN(TIF);
"then"                  return TOKEN(TTHEN);
"else"                  return TOKEN(TELSE);
[ \t\n]                 ;
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_STRING; return TIDENTIFIER;
[0-9]*                  SAVE_NUMBER; return TNUMBER;
"="                     return TOKEN(TEQUAL);
"=="                    return TOKEN(TCEQ);
"!="                    return TOKEN(TCNE);
"<"                     return TOKEN(TCLT);
"<="                    return TOKEN(TCLE);
">"                     return TOKEN(TCGT);
">="                    return TOKEN(TCGE);
"("                     return TOKEN(TLPAREN);
")"                     return TOKEN(TRPAREN);
"{"                     return TOKEN(TLBRACE);
"}"                     return TOKEN(TRBRACE);
"."                     return TOKEN(TDOT);
","                     return TOKEN(TCOMMA);
"+"                     return TOKEN(TPLUS);
"-"                     return TOKEN(TMINUS);
"*"                     return TOKEN(TMUL);
"/"                     return TOKEN(TDIV);
.                       printf("Unknown token!\n"); yyterminate();

%%