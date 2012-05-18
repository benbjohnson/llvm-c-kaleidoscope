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
    LLVMValueRef value = kal_codegen(node, NULL, NULL);
    mu_assert(LLVMGetTypeKind(LLVMTypeOf(value)) == LLVMDoubleTypeKind, "");
    mu_assert(LLVMIsConstant(value), "");
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
    return 0;
}

RUN_TESTS()