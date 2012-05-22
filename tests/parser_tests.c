#include <stdio.h>
#include <string.h>
#include <ast.h>
#include <parser.h>
#include "minunit.h"


//==============================================================================
//
// Test Cases
//
//==============================================================================

//--------------------------------------
// Variable & Number
//--------------------------------------

int test_parse_number() {
    kal_ast_node *node = NULL;
    kal_parse("200", &node);
    mu_assert(node->type == KAL_AST_TYPE_NUMBER, "");
    mu_assert(node->number.value == 200, "");
    kal_ast_node_free(node);
    return 0;
}

int test_parse_variable() {
    kal_ast_node *node = NULL;
    kal_parse("my_var2", &node);
    mu_assert(node->type == KAL_AST_TYPE_VARIABLE, "");
    mu_assert(strcmp(node->variable.name, "my_var2") == 0, "");
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Expressions
//--------------------------------------

int test_parse_addition() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("2+3", &node);
    mu_assert(rc == 0, "");
    mu_assert(node->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->binary_expr.operator == KAL_BINOP_PLUS, "");
    mu_assert(node->binary_expr.lhs->number.value == 2, "");
    mu_assert(node->binary_expr.rhs->number.value == 3, "");
    kal_ast_node_free(node);
    return 0;
}

int test_parse_subtraction() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("200-30", &node);
    mu_assert(rc == 0, "");
    mu_assert(node->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->binary_expr.operator == KAL_BINOP_MINUS, "");
    mu_assert(node->binary_expr.lhs->number.value == 200, "");
    mu_assert(node->binary_expr.rhs->number.value == 30, "");
    kal_ast_node_free(node);
    return 0;
}

int test_parse_multiplication() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("4*8", &node);
    mu_assert(rc == 0, "");
    mu_assert(node->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->binary_expr.operator == KAL_BINOP_MUL, "");
    mu_assert(node->binary_expr.lhs->number.value == 4, "");
    mu_assert(node->binary_expr.rhs->number.value == 8, "");
    kal_ast_node_free(node);
    return 0;
}

int test_parse_division() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("10/2", &node);
    mu_assert(rc == 0, "");
    mu_assert(node->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->binary_expr.operator == KAL_BINOP_DIV, "");
    mu_assert(node->binary_expr.lhs->number.value == 10, "");
    mu_assert(node->binary_expr.rhs->number.value == 2, "");
    kal_ast_node_free(node);
    return 0;
}

int test_parse_parens() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("(12+200)", &node);
    mu_assert(rc == 0, "");
    mu_assert(node->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->binary_expr.operator == KAL_BINOP_PLUS, "");
    mu_assert(node->binary_expr.lhs->number.value == 12, "");
    mu_assert(node->binary_expr.rhs->number.value == 200, "");
    kal_ast_node_free(node);
    return 0;
}

int test_parse_complex() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("4*2+3", &node);
    mu_assert(rc == 0, "");
    mu_assert(node->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->binary_expr.operator == KAL_BINOP_PLUS, "");

    // LHS
    mu_assert(node->binary_expr.lhs->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->binary_expr.lhs->binary_expr.operator == KAL_BINOP_MUL, "");
    mu_assert(node->binary_expr.lhs->binary_expr.lhs->number.value == 4, "");
    mu_assert(node->binary_expr.lhs->binary_expr.rhs->number.value == 2, "");

    // RHS
    mu_assert(node->binary_expr.rhs->number.value == 3, "");
    kal_ast_node_free(node);
    return 0;
}

int test_parse_complex_with_parens() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("4*(2+3)", &node);
    mu_assert(rc == 0, "");
    mu_assert(node->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->binary_expr.operator == KAL_BINOP_MUL, "");

    // LHS
    mu_assert(node->binary_expr.lhs->number.value == 4, "");

    // RHS
    mu_assert(node->binary_expr.rhs->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->binary_expr.rhs->binary_expr.operator == KAL_BINOP_PLUS, "");
    mu_assert(node->binary_expr.rhs->binary_expr.lhs->number.value == 2, "");
    mu_assert(node->binary_expr.rhs->binary_expr.rhs->number.value == 3, "");

    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Function Call
//--------------------------------------

int test_parse_function_call() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("my_func(12, foo+30)", &node);
    mu_assert(rc == 0, "");
    mu_assert(node->type == KAL_AST_TYPE_CALL, "");
    mu_assert(strcmp(node->call.name, "my_func") == 0, "");
    mu_assert(node->call.arg_count == 2, "%d", node->call.arg_count);
    
    // Arg 1
    mu_assert(node->call.args[0]->type == KAL_AST_TYPE_NUMBER, "");
    mu_assert(node->call.args[0]->number.value == 12, "");

    // Arg 2
    mu_assert(node->call.args[1]->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->call.args[1]->binary_expr.operator == KAL_BINOP_PLUS, "");
    mu_assert(strcmp(node->call.args[1]->binary_expr.lhs->variable.name, "foo") == 0, "");
    mu_assert(node->call.args[1]->binary_expr.rhs->number.value == 30, "");
    
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Extern Function
//--------------------------------------

int test_parse_extern() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("extern my_func(foo, bar)", &node);
    mu_assert(rc == 0, "");
    mu_assert(node->type == KAL_AST_TYPE_PROTOTYPE, "");
    mu_assert(strcmp(node->prototype.name, "my_func") == 0, "");
    mu_assert(node->prototype.arg_count == 2, "%d", node->prototype.arg_count);
    mu_assert(strcmp(node->prototype.args[0], "foo") == 0, "");
    mu_assert(strcmp(node->prototype.args[1], "bar") == 0, "");
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Function Definition
//--------------------------------------

int test_parse_function() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("def my_func(foo, bar) foo + bar", &node);
    mu_assert(rc == 0, "");
    mu_assert(node->type == KAL_AST_TYPE_FUNCTION, "");

    // Prototype
    mu_assert(strcmp(node->function.prototype->prototype.name, "my_func") == 0, "");
    mu_assert(node->function.prototype->prototype.arg_count == 2, "");
    mu_assert(strcmp(node->function.prototype->prototype.args[0], "foo") == 0, "");
    mu_assert(strcmp(node->function.prototype->prototype.args[1], "bar") == 0, "");

    // Body
    mu_assert(node->function.body->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->function.body->binary_expr.operator == KAL_BINOP_PLUS, "");
    mu_assert(strcmp(node->function.body->binary_expr.lhs->variable.name, "foo") == 0, "");
    mu_assert(strcmp(node->function.body->binary_expr.rhs->variable.name, "bar") == 0, "");
    
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Conditional
//--------------------------------------

int test_parse_conditional() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("if 1 then 2 else 3", &node);
    mu_assert(rc == 0, "");
    mu_assert(node->type == KAL_AST_TYPE_CONDITIONAL, "");
    mu_assert(node->conditional.condition->number.value == 1, "");
    mu_assert(node->conditional.true_expr->number.value == 2, "");
    mu_assert(node->conditional.false_expr->number.value == 3, "");
    kal_ast_node_free(node);
    return 0;
}



//==============================================================================
//
// Setup
//
//==============================================================================

int all_tests() {
    mu_run_test(test_parse_number);
    mu_run_test(test_parse_variable);
    mu_run_test(test_parse_addition);
    mu_run_test(test_parse_subtraction);
    mu_run_test(test_parse_multiplication);
    mu_run_test(test_parse_division);
    mu_run_test(test_parse_parens);
    mu_run_test(test_parse_complex);
    mu_run_test(test_parse_complex_with_parens);
    mu_run_test(test_parse_function_call);
    mu_run_test(test_parse_extern);
    mu_run_test(test_parse_function);
    mu_run_test(test_parse_conditional);
    return 0;
}

RUN_TESTS()