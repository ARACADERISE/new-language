#include <stdio.h>
#include <string.h>
#include "src/lexer.h"
#include "src/parser.h"
#include "src/visitor.h"

/* 
    The BPL(Brew Programming Language) source code

    Author: ARACADERISE

    The BPL is a statically typed programming language that allows users
    to have a high level ability to work with low level ideals, such like addressing memory blocks
    and or working with memory.
    Think of it as writing C, but in a whole different language and at a higher level.
*/

int main(void) {

    //lexer_T* lexer = init_lexer("varconst{NAME}:\"Hi\" Print();");
    /*lexer_T* lexer = init_lexer(
      "#HEY\n"
      "make [I]age: brand age {memalloc(32);};\n"
      "make [I]age: 15;\n"
      "make [I]ageT: brand ageT {memalloc(36);};\n"
      "varconst{HelloWorld}: \"Hello World!\";\n"
    );*/
    lexer_T* lexer = init_lexer(
        //"make [S]name: \"HEYYY\";\n"
        "varconst{name}:\"Aidan\";\n"
        "make [S]nameW:\"Aidan\";\n"
        "print[any](nameW);\n"
        "make [I]age: n 15{unsigned};\n"
    );
    token_T* token = (void*)0;

    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser);
    
    visitor_T* visitor = init_visitor();
    //visitor_T* visitor = init_visitor();
    visitor_visit(visitor,root);

    return 0;
}