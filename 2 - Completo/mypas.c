/**@<mypas.c>::**/

#include <stdlib.h>
#include <stdio.h>
#include <gettoken.h>
#include <parser.h>

FILE *src;

/*
 * Valida existência de arquivo de entrada e é função principal do programa.
 */
int main(int argc, char const *argv[]) {
    if (argc == 1) {
        // A entrada é o teclado.
        src = stdin;
    } else {
        // Existe um arquivo de entrada.
        src = fopen(argv[1], "r");
        if (src == NULL) {
            fprintf(stderr,
                    "%s: cannot open %s... exiting with error status %d\n",
                    argv[0], argv[1], -1);
            return -1;
        }
    }
    
    lookahead = getToken(src);
    myPas();
    
    printf("\n");
    
    return 0;
}
