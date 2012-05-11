%{
    #include "ast.h"
    kal_ast_node *root;
%}

%union {
    char *string;
    double number;
    kal_ast_node *node;
    int token;
}

%token <string> TIDENTIFIER
%token <number> TNUMBER

%type <node> expr ident number

%right '=' 
%left '-' '+'
%left '*' '/'
%left NEG

%start program

%%

program : /* empty */
        | expr        { root = $1; };

ident   : TIDENTIFIER { $$ = kal_ast_variable_create($1); };

number  : TNUMBER { $$ = kal_ast_number_create($1); };

expr    : number
        | ident
;

%%

