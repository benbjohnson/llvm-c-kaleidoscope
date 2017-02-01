#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/Scalar.h>

#include "ast.h"
#include "parser.h"
#include "codegen.h"

//==============================================================================
//
// Main
//
//==============================================================================

int main(int argc, char **argv)
{
    LLVMModuleRef module = LLVMModuleCreateWithName("kal");
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMExecutionEngineRef engine;

    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    LLVMLinkInMCJIT();

    // Create execution engine.
    char *msg;
    if(LLVMCreateExecutionEngineForModule(&engine, module, &msg) == 1) {
        fprintf(stderr, "%s\n", msg);
        LLVMDisposeMessage(msg);
        return 1;
    }

    // Setup optimizations.
    LLVMPassManagerRef pass_manager =  LLVMCreateFunctionPassManagerForModule(module);
    // LLVMAddTargetData(LLVMGetExecutionEngineTargetData(engine), pass_manager);
    LLVMAddPromoteMemoryToRegisterPass(pass_manager);
    LLVMAddInstructionCombiningPass(pass_manager);
    LLVMAddReassociatePass(pass_manager);
    LLVMAddGVNPass(pass_manager);
    LLVMAddCFGSimplificationPass(pass_manager);
    LLVMInitializeFunctionPassManager(pass_manager);

    // Main REPL loop.
    while(1) {
        // Show prompt.
        fprintf(stderr, "ready > ");

        // Read input.
        char *input = NULL;
        size_t len = 0;

        if(getline(&input, &len, stdin) == -1) {
            fprintf(stderr, "Error reading from stdin\n");
            break;
        }

        // Exit if 'quit' is read.
        if(strcmp(input, "quit\n") == 0) {
            break;
        }

        // Parse
        kal_ast_node *node = NULL;
        int rc = kal_parse(input, &node);
        if(rc != 0) {
            fprintf(stderr, "Parse error\n");
            continue;
        }

        // Wrap in an anonymous function if it's a top-level expression.
        bool is_top_level = (node->type != KAL_AST_TYPE_FUNCTION && node->type != KAL_AST_TYPE_PROTOTYPE);
        if(is_top_level) {
            kal_ast_node *prototype = kal_ast_prototype_create("", NULL, 0);
            node = kal_ast_function_create(prototype, node);
        }

        // Generate node.
        LLVMValueRef value = kal_codegen(node, module, builder);
        if(value == NULL) {
            fprintf(stderr, "Unable to codegen for node\n");
            continue;
        }

        // Dump IR.
        LLVMDumpValue(value);

        // Run it if it's a top level expression.
        if(is_top_level) {
            void *fp = LLVMGetPointerToGlobal(engine, value);
            double (*FP)() = (double (*)())(intptr_t)fp;
            fprintf(stderr, "Evaluted to %f\n", FP());
        }
        // If this is a function then optimize it.
        else if(node->type == KAL_AST_TYPE_FUNCTION) {
            LLVMRunFunctionPassManager(pass_manager, value);
        }

        // Clean up.
        kal_ast_node_free(node);
    }

    // Dump entire module.
    LLVMDumpModule(module);

	LLVMDisposePassManager(pass_manager);
    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);

    return 0;
}
