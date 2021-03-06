#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stdbool.h>
#include <ctype.h>
#include "lexer.h"
#include "parser.h"
#include "token.h"

lexer_T* init_lexer(char *contents) {
    lexer_T* lexer = calloc(1,sizeof(struct LEXER_STRUCT));
    lexer->contents = contents;
    lexer->i = 0;
    lexer->c = contents[lexer->i];
    lexer->line=1;
    lexer->values.print_type = 0;

    /* For Decorators. All false by default */
    lexer->values.isNeg = 1;
    lexer->values.isWrapped = 1;
    lexer->values.isEND = 1;
    lexer->values.hasEndAssignment = 1;
    lexer->values.hasDecorator = 1;
    lexer->values.isReference = 1;
    lexer->values.isDerived = 1;
    lexer->values.isTerminatedMemory = 1;
    lexer->values.isSameMemory = 1;
    lexer->values.isDumped_ReAllocatedMemory = 1;
    lexer->values.isPushValue = 1;
    lexer->values.isTERMINATED = 1;
    lexer->values.terminated_size = 0;

    return lexer;
}
lexer_T* lexer_collect_print_type(lexer_T* lexer, char* type) {
    if(!(lexer->c==']')) {
        printf("\n\nErr[LINE %d]: Expecting type declaration\n\n",lexer->line);
        exit(1);
    }
    if(strcmp(type,"str")==0) {
        lexer->values.print_type = "str";
        lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_STRING,lexer_get_current_char_as_string(lexer)));
    }
    else if(strcmp(type,"char")==0) {
        lexer->values.print_type = "char";
        lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_CHAR,lexer_get_current_char_as_string(lexer)));
    }
    else if(strcmp(type,"int")==0) {
        lexer->values.print_type = "int";
        lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_INT,lexer_get_current_char_as_string(lexer)));
    }
    else if(strcmp(type,"any")==0) {
        lexer->values.print_type = "any";
        lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_A,lexer_get_current_char_as_string(lexer)));
    }
    else {
        printf("\n\nErr[LINE %d]: print's [TYPE] param cannot be left empty, or has a invalid type.\n\n",lexer->line);
        exit(1);
    }
    return lexer;
}
void Comment(lexer_T* lexer) {
    while(lexer->c!='\n'||lexer->c!=10) {
        lexer_advance(lexer);
    }
}
void multi_line_comment(lexer_T* lexer) {
    while(1) {
        lexer_advance(lexer);

        if(lexer->c=='#') {
            lexer_advance(lexer);
            
            if(lexer->c=='-') {
                return lexer_advance(lexer);
            }
            else {
                printf("\n\nErr[LINE %d]: Expecting '/', got %c",lexer->line,lexer->c);
            }
        }
    }
}
void lexer_advance(lexer_T *lexer) {
    
    if(lexer->c != '\0' && lexer->i < strlen(lexer->contents)) {

        lexer->i++;
        lexer->c = lexer->contents[lexer->i];

        /* Getting Line */
        if(lexer->c=='\n'||lexer->c==10)
            lexer->line++;
        
       /* if(lexer->c=='}') {
            lexer->i++;
            lexer->c = lexer->contents[lexer->i];
            printf("%c",lexer->c);

            if(lexer->c==';')
                BrandNeeded=0;
            else BrandNeeded=1;
            
            lexer->i--;
            lexer->c = lexer->contents[lexer->i];
        }*/
                
    }
}
void lexer_skip_whitespace(lexer_T *lexer) {
    while(lexer->c == ' ' || lexer->c == 10) {
        lexer_advance(lexer);
    }
}
char* get_current_as_string(lexer_T* lexer) {
    char* val = calloc(2,sizeof(char));

    val[0]=lexer->c;
    val[1]='\0';

    return val;
}
token_T* lexer_collect_integer_value(lexer_T* lexer) {
    char* value = calloc(1,sizeof(char));
    value[0]='\0';
    int multNeg;

    if(isdigit(lexer->c)) {
        if((!(strcmp(lexer->type,"Integer")==0)&&!(strcmp(lexer->type,"Any")==0)&&!(strcmp(lexer->values.print_type,"any")==0))) {
            if(!(strcmp(lexer->values.print_type,"int")==0)) {
                printf("\n\nErr[LINE %d]: Type of value: %s\nExpected Type: Integer.\n\n",lexer->line,(strlen(lexer->type)>0)?lexer->type : lexer->values.print_type);
                exit(1);
            }
            else goto start;
        }
    }

    start:
    if(lexer->values.isNeg==0) {
        multNeg=-1;
        //lexer_advance(lexer);
    }
    else {multNeg=1;/*lexer_advance(lexer);*/}

    while(isdigit(lexer->c)) {
        char* current = get_current_as_string(lexer);
        strcat(value,current);
        free(current);
        lexer_advance(lexer);
    }
    //lexer_advance(lexer);
    //init_token(TOKEN_TYPE_INT_VALUE, value);

    lexer->values.int_value = atoi(value)*multNeg;
    
    return init_token(TOKEN_INT, value);
}

int lexer_get_bit_assignment(lexer_T* lexer) {
    char* number = calloc(1,sizeof(char));
    number[0]='\0';

    while(isdigit(lexer->c)) {
        char* current_digit = get_current_as_string(lexer);
        strcat(number,current_digit);
        free(current_digit);
        lexer_advance(lexer);
    }
    return atoi(number);
}
token_T* lexer_collect_char_value(lexer_T* lexer) {
    int ammount = 0;
    char* val = calloc(1,sizeof(char));
    val[0]='\0';

    if(!(strcmp(lexer->type,"Char")==0)&&!(strcmp(lexer->type,"Any")==0)) {
        if(!(strcmp(lexer->values.print_type,"char")==0)) {
            printf("\n\nErr[LINE %d]: Print is of type %s. Attempted to print type char\n\n",lexer->line,lexer->values.print_type);
            exit(1);
        }
        else goto start;
    }

    start:
    lexer_advance(lexer);
    while(lexer->c != '\'') {
        if(ammount==1) {
            printf("\n\nErr[LINE %d]: Type [C](%s) has too many values to it.\n\n",lexer->line,lexer->type);
            exit(1);
        }
        char* current_val = get_current_as_string(lexer);
        val = realloc(val, strlen(val) + 2);
        strcat(val,current_val);
        free(current_val);

        lexer_advance(lexer);

        ammount++;
    }
    lexer_advance(lexer);
    return init_token(TOKEN_CHAR,val);
}
int lexer_collect_ammount(lexer_T* lexer) {
    char* ammount = calloc(1,sizeof(char));
    ammount[0]='\0';

    //lexer_skip_whitespace(lexer);

    while(isdigit(lexer->c)) {
        char* cur = get_current_as_string(lexer);
        strcat(ammount, cur);
        free(cur);
        lexer_advance(lexer);
    }
    //lexer_advance(lexer);
    return atoi(ammount);
}
token_T* lexer_get_next_token(lexer_T* lexer) {

    while(lexer->c != '\0' && lexer->i < strlen(lexer->contents)) {
        if(lexer->c == ' ' || lexer->c == 10) 
            lexer_skip_whitespace(lexer);

        if(lexer->c=='N') {
            lexer->values.isNeg=0;
            lexer_advance(lexer);
        }
        if(isdigit(lexer->c)) {
            /*if(lexer->values.isTabbed==0) {
                lexer->values.tabAmmount = lexer_collect_tab_ammount(lexer);
            }
            else */return lexer_collect_integer_value(lexer);
        }

        if(isalnum(lexer->c)) {
            /* Finding type declarations. Will be inside of square brackets */
            if(lexer->c == 'S'&&!(lexer->values.isPushValue==0)) {
                strcpy(lexer->type,"String");
                return lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_STRING,lexer_get_current_char_as_string(lexer)));
            }
            if(lexer->c == 'I'&&!(lexer->values.isPushValue==0)) {
                strcpy(lexer->type,"Integer");
                return lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_INT,lexer_get_current_char_as_string(lexer)));
            }
            if(lexer->c == 'C'&&!(lexer->values.isPushValue==0)) {
                strcpy(lexer->type,"Char");
                return lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_CHAR,lexer_get_current_char_as_string(lexer)));
            }
            if(lexer->c=='A'&&!(lexer->values.isPushValue==0)) {
                strcpy(lexer->type,"Any");
                return lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_A,lexer_get_current_char_as_string(lexer)));
            }
            return lexer_collect_id(lexer);
        }
        
        if(lexer->c=='#') {
            Comment(lexer);
            continue;
        }

        if(lexer->c=='-') {
            lexer_advance(lexer);

            if(lexer->c=='#') {
                multi_line_comment(lexer);
                continue;
            }
            /*else {
                printf("\n\nErr[LINE %d]: Expecting multi line comment '-[', got %c\n\n",lexer->line,lexer->c);
                exit(1);
            }*/
            continue;
        }

        if(lexer->c=='"') return lexer_collect_string(lexer);
        if(lexer->c=='\'') return lexer_collect_char_value(lexer);
        
        switch(lexer->c) {
            //case '"':lexer_collect_string(lexer);break;
            //case '\'':lexer_collect_char_value(lexer);break;
            case ':': {
                lexer_advance(lexer);
                switch(lexer->c) {
                    case ':': return lexer_advance_with_token(lexer,init_token(TOKEN_ADVANCEMENT_OPERATOR,lexer_get_current_char_as_string(lexer))); break;
                    default:return lexer_advance_with_token(lexer,init_token(TOKEN_COLON,lexer_get_current_char_as_string(lexer))); break;
                }
                break;
                //return lexer_advance_with_token(lexer,init_token(TOKEN_COLON,lexer_get_current_char_as_string(lexer))); break;
            }
            case '~': strcpy(lexer->type,"Any");return lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_ANY,lexer_get_current_char_as_string(lexer)));break;
            //case 'n': lexer->values.isNeg=0;return lexer_advance_with_token(lexer,init_token(TOKEN_NEGATIVE_SYMBOL,lexer_get_current_char_as_string(lexer))); break;
            case '[': return lexer_advance_with_token(lexer,init_token(TOKEN_LSQRBRACK,lexer_get_current_char_as_string(lexer))); break;
            case ']': return lexer_advance_with_token(lexer,init_token(TOKEN_RSQRBRACK,lexer_get_current_char_as_string(lexer))); break;
            case '(': return lexer_advance_with_token(lexer,init_token(TOKEN_LPARENT,lexer_get_current_char_as_string(lexer))); break;
            case ')': return lexer_advance_with_token(lexer,init_token(TOKEN_RPARENT,lexer_get_current_char_as_string(lexer))); break;
            case ';': return lexer_advance_with_token(lexer,init_token(TOKEN_SEMI,lexer_get_current_char_as_string(lexer))); break;
            case ',': if(!(lexer->values.isPushValue==0)) return lexer_advance_with_token(lexer,init_token(TOKEN_COMMA,lexer_get_current_char_as_string(lexer)));else lexer_advance(lexer); break;
            case '{': return lexer_advance_with_token(lexer,init_token(TOKEN_LCURL,lexer_get_current_char_as_string(lexer))); break;
            case '}': return lexer_advance_with_token(lexer,init_token(TOKEN_RCURL,lexer_get_current_char_as_string(lexer))); break;
            case '!': return lexer_advance_with_token(lexer,init_token(TOKEN_PREVAR_END_SYMBOL,lexer_get_current_char_as_string(lexer))); break;
            //case '_': return lexer_advance_with_token(lexer,init_token(TOKEN_NO_VALUE,lexer_get_current_char_as_string(lexer))); break;
            case '\0': return init_token(TOKEN_EOF,"\0");break;
            case '=': return lexer_advance_with_token(lexer,init_token(TOKEN_EQUALS,lexer_get_current_char_as_string(lexer))); break;
            default: {
                lexer->i--;
                lexer->c = lexer->contents[lexer->i];
                if(lexer->c!=';'||lexer->c!='!')
                    printf("\n\nErr[LINE %d]: Abrupt end to variable declaration. Expecting ';' or '!'.\n\n",lexer->line);
                else
                    printf("\n\nErr[LINE %d]: Unknown token %c\n\n",lexer->line,lexer->c);
                exit(1);
                break;
            }
        }
    }
    return init_token(TOKEN_EOF, "\0");
}
token_T* lexer_collect_string(lexer_T* lexer) {
    if(!(strcmp(lexer->type,"String")==0)&&!(strcmp(lexer->type,"Any")==0)&&!(strcmp(lexer->values.print_type,"str")==0)&&!(strcmp(lexer->values.print_type,"any")==0)) {
        printf("\n\nErr[LINE %d]: Type of value: String\nExpected Type: %s.\n\n",lexer->line,lexer->values.print_type);
        exit(1);
    }
    lexer_advance(lexer);
    char *value = calloc(1,sizeof(char));
    value[0] = '\0';

    while(lexer->c != '"') {
        if(lexer->c == '\\') {
            lexer_advance(lexer);
            if(lexer->c == 'n') {
                value = realloc(value,(strlen(value)+2/*\n*/)*sizeof(char));
                strcat(value,"\n");
            } else if(lexer->c == '0') {
                value[strlen(value)] = '\0';
                lexer->values.hasEndAssignment = 0;
                lexer_advance(lexer);
                if(lexer->c!='"') {
                    printf("\n\nErr[LINE %d]: Appending after end of string\n\n",lexer->line);
                    exit(1);
                }

                /* Setting lexer pointer back one if the above error is not thrown */
                lexer->i--;
                lexer->c = lexer->contents[lexer->i];
            } else {
                value = realloc(value,(strlen(value)+1/*\*/)*sizeof(char));
                strcat(value,"\\");

                /* Since there was nothing after the '\', we want to set the lexer pointer back one */
                lexer->i--;
                lexer->c = lexer->contents[lexer->i];
            }
        } else {
            char *s = lexer_get_current_char_as_string(lexer);
            value = realloc(value,(strlen(value)+strlen(s)+1)*sizeof(char));
            strcat(value,s);
        }
        lexer_advance(lexer);
    }
    
    advance:
    if(lexer->values.isPushValue==0) lexer->values.pushValue = value;
    lexer_advance(lexer);

    return init_token(TOKEN_STRING,value);
}
token_T* lexer_collect_id(lexer_T* lexer) {
    char *value = calloc(1,sizeof(char));
    value[0] = '\0';

    while(isalnum(lexer->c)) {
        char *s = lexer_get_current_char_as_string(lexer);
        value = realloc(value,(strlen(value)+strlen(s)+1)*sizeof(char));
        strcat(value,s);

        lexer_advance(lexer);
    }

    return init_token(TOKEN_ID,value);
}
token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token) {
    lexer_advance(lexer);

    return token;
}
char* lexer_get_current_char_as_string(lexer_T* lexer) {
    char *str = calloc(2,sizeof(char));
    str[0] = lexer->c;
    str[1] = '\0';

    return str;
}