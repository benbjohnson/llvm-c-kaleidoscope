#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>

#include "codegen.h"

//==============================================================================
//
// Variables
//
//==============================================================================

// The look up for variables by name.
kal_named_value *named_values = NULL;


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
    kal_named_value *val = NULL;
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
// Function Call
//--------------------------------------

// Generates an LLVM value object for a Function Call AST.
//
// node    - The node to generate code for.
//
// Returns an LLVM value reference.
LLVMValueRef kal_codegen_call(kal_ast_node *node, LLVMModuleRef module,
                              LLVMBuilderRef builder)
{
    // Retrieve function.
    LLVMValueRef func = LLVMGetNamedFunction(module, node->call.name);
    
    // Return error if function not found in module.
    if(func == NULL) {
        return NULL;
    }
    
    // Return error if number of arguments doesn't match.
    if(LLVMCountParams(func) != node->call.arg_count) {
        return NULL;
    }
    
    // Evaluate arguments.
    LLVMValueRef *args = malloc(sizeof(LLVMValueRef) * node->call.arg_count);
    unsigned int i;
    unsigned int arg_count = node->call.arg_count;
    for(i=0; i<arg_count; i++) {
        args[i] = kal_codegen(node->call.args[i], module, builder);

        if(args[i] == NULL) {
            free(args);
            return NULL;
        }
    }
    
    // Create call instruction.
    return LLVMBuildCall(builder, func, args, arg_count, "calltmp");
}


//--------------------------------------
// Function Prototype
//--------------------------------------

// Generates an LLVM value object for a Function Prototype AST.
//
// node    - The node to generate code for.
//
// Returns an LLVM value reference.
LLVMValueRef kal_codegen_prototype(kal_ast_node *node, LLVMModuleRef module)
{
    unsigned int i;
    unsigned int arg_count = node->prototype.arg_count;

    // Use an existing definition if one exists.
    LLVMValueRef func = LLVMGetNamedFunction(module, node->prototype.name);
    if(func != NULL) {
        // Verify parameter count matches.
        if(LLVMCountParams(func) != arg_count) {
            fprintf(stderr, "Existing function exists with different parameter count");
            return NULL;
        }
        
        // Verify that the function is empty.
        if(LLVMCountBasicBlocks(func) != 0) {
            fprintf(stderr, "Existing function exists with a body");
            return NULL;
        }
    }
    // Otherwise create a new function definition.
    else {
        // Create argument list.
        LLVMTypeRef *params = malloc(sizeof(LLVMTypeRef) * arg_count);
        for(i=0; i<arg_count; i++) {
            params[i] = LLVMDoubleType();
        }
    
        // Create function type.
        LLVMTypeRef funcType = LLVMFunctionType(LLVMDoubleType(), params, arg_count, 0);
    
        // Create function.
        func = LLVMAddFunction(module, node->prototype.name, funcType);
        LLVMSetLinkage(func, LLVMExternalLinkage);
    }
    
    // Assign arguments to named values lookup.
    for(i=0; i<arg_count; i++) {
        LLVMValueRef param = LLVMGetParam(func, i);
        LLVMSetValueName(param, node->prototype.args[i]);
        
        kal_named_value *val = malloc(sizeof(kal_named_value));
        val->name  = strdup(node->prototype.args[i]);
        val->value = param;
        HASH_ADD_KEYPTR(hh, named_values, val->name, strlen(val->name), val);
    }
    
    return func;
}


//--------------------------------------
// Function
//--------------------------------------

// Generates an LLVM value object for a Function AST.
//
// node    - The node to generate code for.
//
// Returns an LLVM value reference.
LLVMValueRef kal_codegen_function(kal_ast_node *node, LLVMModuleRef module,
                                  LLVMBuilderRef builder)
{
    HASH_CLEAR(hh, named_values);
    
    // Generate the prototype first.
    LLVMValueRef func = kal_codegen(node->function.prototype, module, builder);
    if(func == NULL) {
        return NULL;
    }
    
    // Create basic block.
    LLVMBasicBlockRef block = LLVMAppendBasicBlock(func, "entry");
    LLVMPositionBuilderAtEnd(builder, block);
    
    // Generate body.
    LLVMValueRef body = kal_codegen(node->function.body, module, builder);
    if(body == NULL) {
        LLVMDeleteFunction(func);
        return NULL;
    }
    
    // Insert body as return vale.
    LLVMBuildRet(builder, body);
    
    // Verify function.
    if(LLVMVerifyFunction(func, LLVMPrintMessageAction) == 1) {
        fprintf(stderr, "Invalid function");
        LLVMDeleteFunction(func);
        return NULL;
    }
    
    return func;
}


//--------------------------------------
// If Expression
//--------------------------------------

// Generates an LLVM value object for an If Expression AST.
//
// node    - The node to generate code for.
//
// Returns an LLVM value reference.
LLVMValueRef kal_codegen_if_expr(kal_ast_node *node, LLVMModuleRef module,
                                 LLVMBuilderRef builder)
{
    // Generate the condition.
    LLVMValueRef condition = kal_codegen(node->if_expr.condition, module, builder);
    if(condition == NULL) {
        return NULL;
    }
    
    // Convert condition to bool.
    LLVMValueRef zero = LLVMConstReal(LLVMDoubleType(), 0);
    condition = LLVMBuildFCmp(builder, LLVMRealONE, condition, zero, "ifcond");

    // Retrieve function.
    LLVMValueRef func = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));
    
    // Generate true/false expr and merge.
    LLVMBasicBlockRef then_block = LLVMAppendBasicBlock(func, "then");
    LLVMBasicBlockRef else_block = LLVMAppendBasicBlock(func, "else");
    LLVMBasicBlockRef merge_block = LLVMAppendBasicBlock(func, "ifcont");
    
    LLVMBuildCondBr(builder, condition, then_block, else_block);

    // Generate 'then' block.
    LLVMPositionBuilderAtEnd(builder, then_block);
    LLVMValueRef then_value = kal_codegen(node->if_expr.true_expr, module, builder);
    if(then_value == NULL) {
        return NULL;
    }
    
    LLVMBuildBr(builder, merge_block);
    then_block = LLVMGetInsertBlock(builder);
    
    LLVMPositionBuilderAtEnd(builder, else_block);
    LLVMValueRef else_value = kal_codegen(node->if_expr.false_expr, module, builder);
    if(else_value == NULL) {
        return NULL;
    }
    LLVMBuildBr(builder, merge_block);
    else_block = LLVMGetInsertBlock(builder);

    LLVMPositionBuilderAtEnd(builder, merge_block);
    LLVMValueRef phi = LLVMBuildPhi(builder, LLVMDoubleType (), "");
    LLVMAddIncoming(phi, &then_value, &then_block, 1);
    LLVMAddIncoming(phi, &else_value, &else_block, 1);
    
    return phi;
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
            return kal_codegen_call(node, module, builder);
        }
        case KAL_AST_TYPE_PROTOTYPE: {
            return kal_codegen_prototype(node, module);
        }
        case KAL_AST_TYPE_FUNCTION: {
            return kal_codegen_function(node, module, builder);
        }
        case KAL_AST_TYPE_IF_EXPR: {
            return kal_codegen_if_expr(node, module, builder);
        }
    }
    
    return NULL;
}


//--------------------------------------
// Utility
//--------------------------------------

// Clears the named variables.
void kal_codegen_reset()
{
    HASH_CLEAR(hh, named_values);
}

// Clears the named variables.
kal_named_value *kal_codegen_named_value(const char *name)
{
    kal_named_value *val = NULL;
    HASH_FIND_STR(named_values, name, val);
    return val;
}
