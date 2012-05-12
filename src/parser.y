%{
    #include "stdio.h"
    #include "ast.h"
    #include "lexer.h"
    kal_ast_node *root;
    extern int yylex();
    void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

%debug

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
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV

%type <node> expr ident number

%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : /* empty */
        | expr        { root = $1; };

ident   : TIDENTIFIER { $$ = kal_ast_variable_create($1); };

number  : TNUMBER { $$ = kal_ast_number_create($1);};

expr    : expr TPLUS expr   { $$ = kal_ast_binary_expr_create(KAL_BINOP_PLUS, $1, $3); }
        | expr TMINUS expr  { $$ = kal_ast_binary_expr_create(KAL_BINOP_MINUS, $1, $3); }
        | expr TMUL expr    { $$ = kal_ast_binary_expr_create(KAL_BINOP_MUL, $1, $3); }
        | expr TDIV expr    { $$ = kal_ast_binary_expr_create(KAL_BINOP_DIV, $1, $3); }
        | number
        | ident
        | TLPAREN expr TRPAREN { $$ = $2 }
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
    if(rc == 0) {
        *node = root;
        return 0;
    }
    // Otherwise return error.
    else {
        return -1;
    }
}
