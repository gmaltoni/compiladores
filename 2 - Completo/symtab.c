#include <stdio.h>
#include <gettoken.h>
#include <symtab.h>
#include <string.h>

SYMTAB symtab[MAXSYMENTRIES];

int symtab_nextentry = 1;

/*
 * Procura por uma entrada na tabela de símbolos.
 */
int symtabLookup(char const *query) {
    int i;
    
    for (i = 1; i < symtab_nextentry; i++) {
        if (strcmp(symtab[i].name, query) == 0)
            return i;
    }
    
    return 0;
}

/*
 * Adiciona uma entrada na tabela de símbolos.
 */
int symtabAppend(char const *newname) {
    int i = symtabLookup(newname);
    
    if (i == 0) {
        strcpy(symtab[symtab_nextentry].name, newname);
        return symtab_nextentry++;
    }
    
    return 0; // Símbolo duplicado.
}

/*
 * Coloca um tipo para um conjunto de símbolos.
 */
void symtabSetType(int vtype, int a) {
    int i;
    
    for (i = a; i < symtab_nextentry; i++) {
        symtab[i].vtype = vtype;
    }
}
