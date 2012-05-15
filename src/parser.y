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

%code top {
    void free_args(void **args, int count);
}

%union {
    char *string;
    double number;
    kal_ast_node *node;
    struct {
        kal_ast_node **args;
        int count;
    } call_args;
    struct {
        char **args;
        int count;
    } proto_args;
    int token;
}

%token <string> TIDENTIFIER
%token <number> TNUMBER
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV
%token <token> TEXTERN TDEF

%type <node> expr ident number call prototype extern_func function
%type <call_args> call_args
%type <proto_args> proto_args

%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : /* empty */
        | extern_func { root = $1; }
        | function    { root = $1; }
        | expr        { root = $1; }
;

ident   : TIDENTIFIER { $$ = kal_ast_variable_create($1); free($1); };

number  : TNUMBER { $$ = kal_ast_number_create($1);};

function : TDEF prototype expr   { $$ = kal_ast_function_create($2, $3); };

call  : TIDENTIFIER TLPAREN call_args TRPAREN { $$ = kal_ast_call_create($1, $3.args, $3.count); free($1); free($3.args); };

call_args : /* empty */     { $$.count = 0; $$.args = NULL; }
          | expr            { $$.count = 1; $$.args = malloc(sizeof(kal_ast_node*)); $$.args[0] = $1; }
          | call_args TCOMMA expr  { $1.count++; $1.args = realloc($1.args, sizeof(kal_ast_node*) * $1.count); $1.args[$1.count-1] = $3; $$ = $1; }
;

prototype : TIDENTIFIER TLPAREN proto_args TRPAREN { $$ = kal_ast_prototype_create($1, $3.args, $3.count); free($1); free_args((void**)$3.args, $3.count); };

proto_args : /* empty */     { $$.count = 0; $$.args = NULL; }
           | TIDENTIFIER     { $$.count = 1; $$.args = malloc(sizeof(char*)); $$.args[0] = strdup($1); }
           | proto_args TCOMMA TIDENTIFIER  { $1.count++; $1.args = realloc($1.args, sizeof(char*) * $1.count); $1.args[$1.count-1] = strdup($3); $$ = $1; }
;

extern_func : TEXTERN prototype  { $$ = $2; };

expr    : expr TPLUS expr   { $$ = kal_ast_binary_expr_create(KAL_BINOP_PLUS, $1, $3); }
        | expr TMINUS expr  { $$ = kal_ast_binary_expr_create(KAL_BINOP_MINUS, $1, $3); }
        | expr TMUL expr    { $$ = kal_ast_binary_expr_create(KAL_BINOP_MUL, $1, $3); }
        | expr TDIV expr    { $$ = kal_ast_binary_expr_create(KAL_BINOP_DIV, $1, $3); }
        | number
        | ident
        | call
        | TLPAREN expr TRPAREN { $$ = $2; }
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
    // yydebug = 1;
    
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

// Frees an array and all the elements of the array.
//
// args - The array to free.
// count - The number of elements in the array.
void free_args(void **args, int count)
{
    int i;
    for(i=0; i<count; i++) {
        free(args[i]);
    }

    free(args);
}
