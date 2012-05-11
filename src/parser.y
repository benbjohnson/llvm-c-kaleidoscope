%{
    #include "stdio.h"
    #include "ast.h"
    #include "lexer.h"
    kal_ast_node *root;
    extern int yylex();
    void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

%code provides {
    int kal_parse(char *text, kal_ast_node **node);
}

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


//==============================================================================
//
// Functions
//
//==============================================================================

// Parses a string that contains Kaleidoscope program text.
//
// text - The text containing the kaleidoscope program.
// node - The pointer to where the root AST node should be returned.
//
// Returns 0 if successful, otherwise returns -1.
int kal_parse(char *text, kal_ast_node **node)
{
    // Parse using Bison.
    YY_BUFFER_STATE buffer = yy_scan_string(text);
    int rc = yyparse();
    yy_delete_buffer(buffer);
    
    // If parse was successful, return root node.
    if(rc) {
        *node = root;
        return 0;
    }
    // Otherwise return error.
    else {
        return -1;
    }
}
