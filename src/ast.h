#ifndef AST_H
#define AST_H
#include <stdlib.h>

typedef struct AST_STRUCT {
    enum {
        AST_VARIABLE_DEFINITION,
        AST_PREVAR_DEFINITION,
        AST_VARIABLE,
        AST_PREVAR,
        AST_FUNCTION_CALL,
        AST_STRING,
        AST_COMPOUND,
        AST_NOOP
    } type;

    /* For AST_VARIBALE_DEFINITION */
    char* variable_definition_variable_name;
    struct AST_STRUCT* variable_definition_value;

    /* For AST_PREVAR_DEFINITION */
    char* PreVar_name;
    struct AST_STRUCT* PreVar_value;

    /* For AST_VARIABLE */
    char* variable_name;

    /* For AST_PREVAR */
    char* PreVar_variable_name;

    /* For AST_FUNCTION_CALL */
    char *function_call_name;
    struct AST_STRUCT** function_call_arguments;
    size_t function_call_arguments_size;

    /* For AST_STRING */
    char* string_value;

    /* For AST_COMPOUND */
    struct AST_STRUCT** compound_value;
    size_t compound_size;
} AST_T;

AST_T* init_ast(int type);

#endif