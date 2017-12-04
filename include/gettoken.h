/**@<gettoken.h>::**/
#define MAXSTRLEN 	1024

extern char *tokeninfo[]; 

extern char lexeme[];

enum {
    ID = 1024,
    DEC,
    FLT,
    SCI,
    NEQ,
    GEQ,
    LEQ,    
    ASGNM,
    CH,
    ST
};

int  getToken(FILE *);
