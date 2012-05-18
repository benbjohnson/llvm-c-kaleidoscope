#ifndef _codegen_h
#define _codegen_h

#include <llvm-c/Core.h>
#include "ast.h"

//==============================================================================
//
// Functions
//
//==============================================================================

LLVMValueRef kal_codegen(kal_ast_node *node, LLVMModuleRef module,
    LLVMBuilderRef builder);

LLVMValueRef kal_codegen_function(kal_ast_node *node, LLVMModuleRef module,
    LLVMBuilderRef builder);

#endif