#include <stdio.h>
#include <string.h>
#include <ast.h>
#include <parser.h>
#include "minunit.h"


//==============================================================================
//
// Test Cases
//
//==============================================================================

//--------------------------------------
// Parse Number
//--------------------------------------

int test_parse_number() {
    kal_ast_node *node = NULL;
    int rc = kal_parse("200", &node);
    mu_assert(rc == 0, "");
    mu_assert(node != NULL, "");
    mu_assert(node->type == KAL_AST_TYPE_NUMBER, "");
    mu_assert(node->number.value == 200, "");
    kal_ast_node_free(node);
    return 0;
}



//==============================================================================
//
// Setup
//
//==============================================================================

int all_tests() {
    mu_run_test(test_parse_number);
    return 0;
}

RUN_TESTS()