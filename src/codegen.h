#ifndef _codegen_h
#define _codegen_h

#include <llvm-c/Core.h>
#include "ast.h"
#include "uthash.h"


//==============================================================================
//
// Typedefs
//
//==============================================================================

// Used to hold references to arguments by name.
typedef struct kal_named_value {
    const char *name;             
    LLVMValueRef value;
    UT_hash_handle hh;
} kal_named_value;


//==============================================================================
//
// Functions
//
//==============================================================================

//--------------------------------------
// Codegen
//--------------------------------------

LLVMValueRef kal_codegen(kal_ast_node *node, LLVMModuleRef module,
    LLVMBuilderRef builder);


//--------------------------------------
// Utility
//--------------------------------------

void kal_codegen_reset();

kal_named_value *kal_codegen_named_value(const char *name);


#endif