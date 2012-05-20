#include <stdio.h>
#include <string.h>
#include <ast.h>
#include <codegen.h>
#include <llvm-c/Core.h>
#include "minunit.h"


//==============================================================================
//
// Test Cases
//
//==============================================================================

//--------------------------------------
// Number
//--------------------------------------

int test_kal_codegen_number() {
    kal_ast_node *node = kal_ast_number_create(10);
    LLVMValueRef value = kal_codegen(node, NULL, NULL);
    LLVMTypeRef type = LLVMTypeOf(value);
    mu_assert(LLVMGetTypeKind(type) == LLVMDoubleTypeKind, "");
    mu_assert(LLVMIsConstant(value), "");
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Binary Expression
//--------------------------------------

int test_kal_codegen_binary_expr() {
    LLVMModuleRef module = LLVMModuleCreateWithName("kal");
    LLVMBuilderRef builder = LLVMCreateBuilder();
    kal_ast_node *lhs = kal_ast_number_create(20);
    kal_ast_node *rhs = kal_ast_number_create(30);
    kal_ast_node *node = kal_ast_binary_expr_create(KAL_BINOP_PLUS, lhs, rhs);
    LLVMValueRef value = kal_codegen(node, module, builder);
    mu_assert(LLVMGetTypeKind(LLVMTypeOf(value)) == LLVMDoubleTypeKind, "");
    mu_assert(LLVMIsConstant(value), "");
    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Prototype
//--------------------------------------

int test_kal_codegen_prototype() {
    kal_named_value *val;
    unsigned int arg_count = 3;
    char **args = malloc(sizeof(char*) * arg_count);
    args[0] = "foo";
    args[1] = "bar";
    args[2] = "baz";
    
    LLVMModuleRef module = LLVMModuleCreateWithName("kal");
    LLVMBuilderRef builder = LLVMCreateBuilder();
    kal_ast_node *node = kal_ast_prototype_create("my_func", args, 3);

    kal_codegen_reset();
    LLVMValueRef value = kal_codegen(node, module, builder);

    mu_assert(value != NULL, "");
    mu_assert(LLVMGetNamedFunction(module, "my_func") == value, "");
    mu_assert(LLVMCountParams(value) == 3, "");

    val = kal_codegen_named_value("foo");
    mu_assert(val->value == LLVMGetParam(value, 0), "");
    mu_assert(LLVMGetTypeKind(LLVMTypeOf(LLVMGetParam(value, 0))) == LLVMDoubleTypeKind, "");

    val = kal_codegen_named_value("bar");
    mu_assert(val->value == LLVMGetParam(value, 1), "");
    mu_assert(LLVMGetTypeKind(LLVMTypeOf(LLVMGetParam(value, 1))) == LLVMDoubleTypeKind, "");

    val = kal_codegen_named_value("baz");
    mu_assert(val->value == LLVMGetParam(value, 2), "");
    mu_assert(LLVMGetTypeKind(LLVMTypeOf(LLVMGetParam(value, 2))) == LLVMDoubleTypeKind, "");

    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
    kal_ast_node_free(node);
    return 0;
}


//--------------------------------------
// Function
//--------------------------------------

int test_kal_codegen_function() {
    kal_named_value *val;
    unsigned int arg_count = 1;
    char **args = malloc(sizeof(char*) * arg_count);
    args[0] = "foo";
    
    LLVMModuleRef module = LLVMModuleCreateWithName("kal");
    LLVMBuilderRef builder = LLVMCreateBuilder();
    kal_ast_node *prototype = kal_ast_prototype_create("my_func", args, arg_count);
    kal_ast_node *lhs = kal_ast_variable_create("foo");
    kal_ast_node *rhs = kal_ast_number_create(20);
    kal_ast_node *body = kal_ast_binary_expr_create(KAL_BINOP_PLUS, lhs, rhs);
    kal_ast_node *node = kal_ast_function_create(prototype, body);

    kal_codegen_reset();
    LLVMValueRef value = kal_codegen(node, module, builder);

    mu_assert(value != NULL, "");
    mu_assert(LLVMGetNamedFunction(module, "my_func") == value, "");
    mu_assert(LLVMCountParams(value) == 1, "");

    val = kal_codegen_named_value("foo");
    mu_assert(val->value == LLVMGetParam(value, 0), "");
    mu_assert(LLVMGetTypeKind(LLVMTypeOf(LLVMGetParam(value, 0))) == LLVMDoubleTypeKind, "");

    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
    kal_ast_node_free(node);
    return 0;
}



//==============================================================================
//
// Setup
//
//==============================================================================

int all_tests() {
    mu_run_test(test_kal_codegen_number);
    mu_run_test(test_kal_codegen_binary_expr);
    mu_run_test(test_kal_codegen_prototype);
    mu_run_test(test_kal_codegen_function);
    return 0;
}

RUN_TESTS()