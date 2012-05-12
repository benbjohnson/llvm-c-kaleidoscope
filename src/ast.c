#include <stdlib.h>
#include <string.h>
#include "ast.h"

//==============================================================================
//
// Functions
//
//==============================================================================

//--------------------------------------
// Number AST
//--------------------------------------

// Creates an AST node for a number.
//
// value - The value of the AST.
//
// Returns a Number AST Node.
kal_ast_node *kal_ast_number_create(double value)
{
    kal_ast_node *node = malloc(sizeof(kal_ast_node));
    node->type = KAL_AST_TYPE_NUMBER;
    node->number.value = value;
    return node;
}


//--------------------------------------
// Variable AST
//--------------------------------------

// Creates an AST node for a variable.
//
// name - The name of the variable.
//
// Returns a Variable AST Node.
kal_ast_node *kal_ast_variable_create(char *name)
{
    kal_ast_node *node = malloc(sizeof(kal_ast_node));
    node->type = KAL_AST_TYPE_VARIABLE;
    node->variable.name = strdup(name);
    return node;
}


//--------------------------------------
// Binary Expression AST
//--------------------------------------

// Creates an AST node for a binary expression.
//
// op  - The operation being performed.
// lhs - The AST node for the left hand side of the expression.
// rhs - The AST node for the right hand side of the expression.
//
// Returns a Binary Expression AST Node.
kal_ast_node *kal_ast_binary_expr_create(kal_ast_binop_e operator,
                                         kal_ast_node *lhs,
                                         kal_ast_node *rhs)
{
    kal_ast_node *node = malloc(sizeof(kal_ast_node));
    node->type = KAL_AST_TYPE_BINARY_EXPR;
    node->binary_expr.operator = operator;
    node->binary_expr.lhs      = lhs;
    node->binary_expr.rhs      = rhs;
    return node;
}


//--------------------------------------
// Function Call AST
//--------------------------------------

// Creates an AST node for a function call.
//
// name      - The name of the function being called.
// args      - A list of AST node expressions passed as arguments.
// arg_count - The number of arguments.
//
// Returns a Function Call AST Node.
kal_ast_node *kal_ast_call_create(char *callee, kal_ast_node **args,
                                  int arg_count)
{
    kal_ast_node *node = malloc(sizeof(kal_ast_node));
    node->type = KAL_AST_TYPE_CALL;
    node->call.callee = strdup(callee);

    // Shallow copy arguments.
    node->prototype.args = malloc(sizeof(kal_ast_node*) * arg_count);
    memcpy(node->prototype.args, args, sizeof(kal_ast_node*) * arg_count);
    node->prototype.arg_count = arg_count;

    return node;
}


//--------------------------------------
// Function Prototype AST
//--------------------------------------

// Creates an AST node for a function prototype.
//
// name      - The name of the function.
// args      - A list of argument names.
// arg_count - The number of arguments.
//
// Returns a Function Prototype AST Node.
kal_ast_node *kal_ast_prototype_create(char *name, char **args,
                                       int arg_count)
{
    int i;

    kal_ast_node *node = malloc(sizeof(kal_ast_node));
    node->type = KAL_AST_TYPE_PROTOTYPE;
    node->prototype.name = strdup(name);
    
    // Copy arguments.
    node->prototype.args = malloc(sizeof(char*) * arg_count);
    for(i=0; i<arg_count; i++) {
        node->prototype.args[i] = strdup(args[i]);
    }
    node->prototype.arg_count = arg_count;

    return node;
}


//--------------------------------------
// Function AST
//--------------------------------------

// Creates an AST node for a function declaration.
//
// prototype - The definition for the function.
// body      - The body expression.
//
// Returns a Function AST Node.
kal_ast_node *kal_ast_function_create(kal_ast_node *prototype,
                                      kal_ast_node *body)
{
    kal_ast_node *node = malloc(sizeof(kal_ast_node));
    node->type = KAL_AST_TYPE_FUNCTION;
    node->function.prototype = prototype;
    node->function.body      = body;
    return node;
}


//--------------------------------------
// Node Lifecycle
//--------------------------------------

// Recursively frees an AST node.
//
// node - The node to free.
void kal_ast_node_free(kal_ast_node *node)
{
    int i;
    
    if(!node) return;
    
    // Recursively free dependent data.
    switch(node->type) {
        case KAL_AST_TYPE_NUMBER: break;
        case KAL_AST_TYPE_VARIABLE: {
            if(node->variable.name) free(node->variable.name);
            break;
        }
        case KAL_AST_TYPE_BINARY_EXPR: {
            if(node->binary_expr.lhs) kal_ast_node_free(node->binary_expr.lhs);
            if(node->binary_expr.rhs) kal_ast_node_free(node->binary_expr.rhs);
            break;
        }
        case KAL_AST_TYPE_CALL: {
            if(node->call.callee) free(node->call.callee);
            for(i=0; i<node->call.arg_count; i++) {
                kal_ast_node_free(node->call.args[i]);
            }
            free(node->call.args);
            break;
        }
        case KAL_AST_TYPE_PROTOTYPE: {
            if(node->prototype.name) free(node->prototype.name);
            for(i=0; i<node->prototype.arg_count; i++) {
                free(node->prototype.args[i]);
            }
            free(node->prototype.args);
            break;
        }
        case KAL_AST_TYPE_FUNCTION: {
            if(node->function.prototype) kal_ast_node_free(node->function.prototype);
            if(node->function.body) kal_ast_node_free(node->function.body);
            break;
        }
    }
    
    free(node);
}

