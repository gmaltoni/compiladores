/**@<keywords.c>::**/

#include <keywords.h>
#include <string.h>

// Conjunto de palavras reservadas.

char *keyword[] = {
    "begin",
    "var",
    "byte",
    "word",
    "integer",
    "longint",
    "real",
    "double",
    "boolean",
    "char",
    "string",
    "if",
    "then",
    "else",
    "while",
    "do",
    "repeat",
    "until",
    "not",
    "div",
    "mod",
    "and",
    "or",
    "true",
    "false",
    "end"
};

/*
 * Verifica se o identificador é uma palavra reservada.
 */
int isKeyword(char const *identifier) {
    int i;
    
    for (i = BEGIN; i <= END; i++) {
        if (strcmp(keyword[i - BEGIN], identifier) == 0)
	    return i;
    }
    
    return 0;
}
