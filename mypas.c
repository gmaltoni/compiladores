/**@<parser-main.c>::**/

#include <stdlib.h>
#include <stdio.h>
#include <gettoken.h>
#include <parser.h>

FILE           *src;

int
main(int argc, char const *argv[])
{
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

    /*
     * while(1) { int token = gettoken(src);
     * 
     * if (token == EOF) // fim do arquivo { break; }
     * 
     * printf("%d ", token); }
     */

    lookahead = gettoken(src);
    mypas();

    printf("\n");

    return 0;

}
