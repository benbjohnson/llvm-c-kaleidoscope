#include <stdio.h>
#include <stdlib.h>

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
        
        // Generate node.
        LLVMValueRef value = kal_codegen(node, module, builder);
        if(value == NULL) {
            fprintf(stderr, "Unable to codegen for node\n");
            continue;
        }
        
        // Dump IR.
        LLVMDumpValue(value);

        // Clean up.
        kal_ast_node_free(node);
    }
    
    // Dump entire module.
    LLVMDumpModule(module);
    
    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);

    return 0;
}

