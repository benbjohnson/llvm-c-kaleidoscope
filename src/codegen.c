#include <stdlib.h>
#include <string.h>
#include <llvm-c/Core.h>

#include "codegen.h"


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
            break;
        }
        case KAL_AST_TYPE_VARIABLE: {
            break;
        }
        case KAL_AST_TYPE_BINARY_EXPR: {
            break;
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
