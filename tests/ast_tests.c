#include <stdio.h>
#include <string.h>
#include <ast.h>
#include "minunit.h"


//==============================================================================
//
// Test Cases
//
//==============================================================================

//--------------------------------------
// Number AST
//--------------------------------------

int test_kal_ast_number_create() {
    kal_ast_node *node = kal_ast_number_create(10);
    mu_assert(node->type == KAL_AST_TYPE_NUMBER, "");
    mu_assert(node->number.value == 10, "");
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Variable AST
//--------------------------------------

int test_kal_ast_variable_create() {
    kal_ast_node *node = kal_ast_variable_create("foo");
    mu_assert(node->type == KAL_AST_TYPE_VARIABLE, "");
    mu_assert(strcmp(node->variable.name, "foo") == 0, "");
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Binary Expression AST
//--------------------------------------

int test_kal_ast_binary_expr_create() {
    kal_ast_node *number = kal_ast_number_create(20);
    kal_ast_node *variable = kal_ast_variable_create("bar");
    kal_ast_node *node = kal_ast_binary_expr_create(KAL_BINOP_PLUS, number, variable);
    mu_assert(node->type == KAL_AST_TYPE_BINARY_EXPR, "");
    mu_assert(node->binary_expr.operator == KAL_BINOP_PLUS, "");
    mu_assert(node->binary_expr.lhs == number, "");
    mu_assert(node->binary_expr.rhs == variable, "");
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Funtion Call AST
//--------------------------------------

int test_kal_ast_call_create() {
    kal_ast_node *args[2];
    args[0] = kal_ast_number_create(100);
    args[1] = kal_ast_number_create(200);
    kal_ast_node *node = kal_ast_call_create("baz", args, 2);
    mu_assert(node->type == KAL_AST_TYPE_CALL, "");
    mu_assert(strcmp(node->call.name, "baz") == 0, "");
    mu_assert(node->call.args[0] == args[0], "");
    mu_assert(node->call.args[1] == args[1], "");
    mu_assert(node->call.arg_count == 2, "");
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Funtion Prototype AST
//--------------------------------------

int test_kal_ast_prototype_create() {
    char *args[2];
    args[0] = "foo";
    args[1] = "bar";
    kal_ast_node *node = kal_ast_prototype_create("baz", args, 2);
    mu_assert(node->type == KAL_AST_TYPE_PROTOTYPE, "");
    mu_assert(strcmp(node->prototype.name, "baz") == 0, "");
    mu_assert(strcmp(node->prototype.args[0], "foo") == 0, "");
    mu_assert(strcmp(node->prototype.args[1], "bar") == 0, "");
    mu_assert(node->prototype.arg_count == 2, "");
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Funtion Prototype AST
//--------------------------------------

int test_kal_ast_function_create() {
    kal_ast_node *prototype = kal_ast_prototype_create("baz", NULL, 0);
    kal_ast_node *body = kal_ast_variable_create("foo");
    kal_ast_node *node = kal_ast_function_create(prototype, body);
    mu_assert(node->type == KAL_AST_TYPE_FUNCTION, "");
    mu_assert(node->function.prototype == prototype, "");
    mu_assert(node->function.body == body, "");
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// If Expression AST
//--------------------------------------

int test_kal_ast_if_expr_create() {
    kal_ast_node *condition = kal_ast_number_create(1);
    kal_ast_node *true_expr = kal_ast_number_create(2);
    kal_ast_node *false_expr = kal_ast_number_create(3);
    kal_ast_node *node = kal_ast_if_expr_create(condition, true_expr, false_expr);
    mu_assert(node->type == KAL_AST_TYPE_IF_EXPR, "");
    mu_assert(node->if_expr.condition == condition, "");
    mu_assert(node->if_expr.true_expr == true_expr, "");
    mu_assert(node->if_expr.false_expr == false_expr, "");
    kal_ast_node_free(node);
    return 0;
}


//==============================================================================
//
// Setup
//
//==============================================================================

int all_tests() {
    mu_run_test(test_kal_ast_number_create);
    mu_run_test(test_kal_ast_variable_create);
    mu_run_test(test_kal_ast_binary_expr_create);
    mu_run_test(test_kal_ast_call_create);
    mu_run_test(test_kal_ast_prototype_create);
    mu_run_test(test_kal_ast_function_create);
    mu_run_test(test_kal_ast_if_expr_create);
    return 0;
}

RUN_TESTS()