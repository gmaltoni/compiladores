/**@<mypas.c>::**/

#include <stdlib.h>
#include <stdio.h>
#include <gettoken.h>
#include <parser.h>

FILE *src;

int main(int argc, char const *argv[]) {
    if (argc == 1) {
        src = stdin;
    } else {
        src = fopen(argv[1], "r");
        if (src == NULL) {
            fprintf(stderr, 
                    "%s: cannot open %s... exiting",
                    argv[0], argv[1], " with error status %d\n", -1);
            return -1;
        }
    }
    
    lookahead = getToken(src);
    mypas();

    printf("\n");

    return 0;
}
