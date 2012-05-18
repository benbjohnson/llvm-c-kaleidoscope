#include <stdlib.h>
#include <string.h>
#include <llvm-c/Core.h>

#include "codegen.h"
#include "uthash.h"

//==============================================================================
//
// Variables
//
//==============================================================================

struct kal_named_value {
    const char *name;             
    LLVMValueRef value;
    UT_hash_handle hh;
};

struct kal_named_value *named_values = NULL;


//==============================================================================
//
// Functions
//
//==============================================================================

//--------------------------------------
// Number
//--------------------------------------

// Generates an LLVM value object for a Number AST.
//
// node    - The node to generate code for.
//
// Returns an LLVM value reference.
LLVMValueRef kal_codegen_number(kal_ast_node *node)
{
    return LLVMConstReal(LLVMDoubleType(), node->number.value);
}


//--------------------------------------
// Variable
//--------------------------------------

// Generates an LLVM value object for a Variable AST.
//
// node    - The node to generate code for.
//
// Returns an LLVM value reference.
LLVMValueRef kal_codegen_variable(kal_ast_node *node)
{
    // Lookup variable reference.
    struct kal_named_value *val = NULL;
    HASH_FIND_STR(named_values, node->variable.name, val);
    
    if(val != NULL) {
        return val->value;
    }
    else {
        return NULL;
    }
}


//--------------------------------------
// Variable
//--------------------------------------

// Generates an LLVM value object for a Binary Expression AST.
//
// node    - The node to generate code for.
//
// Returns an LLVM value reference.
LLVMValueRef kal_codegen_binary_expr(kal_ast_node *node, LLVMModuleRef module,
                                     LLVMBuilderRef builder)
{
    // Evaluate left and right hand values.
    LLVMValueRef lhs = kal_codegen(node->binary_expr.lhs, module, builder);
    LLVMValueRef rhs = kal_codegen(node->binary_expr.rhs, module, builder);

    // Return NULL if one of the sides is invalid.
    if(lhs == NULL || rhs == NULL) {
        return NULL;
    }
    
    // Create different IR code depending on the operator.
    switch(node->binary_expr.operator) {
        case KAL_BINOP_PLUS: {
            return LLVMBuildFAdd(builder, lhs, rhs, "addtmp");
        }
        case KAL_BINOP_MINUS: {
            return LLVMBuildFSub(builder, lhs, rhs, "subtmp");
        }
        case KAL_BINOP_MUL: {
            return LLVMBuildFMul(builder, lhs, rhs, "multmp");
        }
        case KAL_BINOP_DIV: {
            return LLVMBuildFDiv(builder, lhs, rhs, "divtmp");
        }
    }
    
    return NULL;
}


//--------------------------------------
// Code Generation
//--------------------------------------

// Recursively generates LLVM objects to build the code.
//
// node    - The node to generate code for.
// module  - The module that the code is being generated for.
// builder - The LLVM builder that is creating the IR.
//
// Returns an LLVM value reference.
LLVMValueRef kal_codegen(kal_ast_node *node, LLVMModuleRef module,
                         LLVMBuilderRef builder)
{
    // Recursively free dependent data.
    switch(node->type) {
        case KAL_AST_TYPE_NUMBER: {
            return kal_codegen_number(node);
        }
        case KAL_AST_TYPE_VARIABLE: {
            return kal_codegen_variable(node);
        }
        case KAL_AST_TYPE_BINARY_EXPR: {
            return kal_codegen_binary_expr(node, module, builder);
        }
        case KAL_AST_TYPE_CALL: {
            break;
        }
        case KAL_AST_TYPE_PROTOTYPE: {
            break;
        }
        case KAL_AST_TYPE_FUNCTION: {
            break;
        }
    }
    
    return NULL;
}
