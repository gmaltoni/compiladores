/**@<gettoken.h>::**/
#define MAXSTRLEN 	1024
#define FLT_MAX		3.4E38
#define FLT_MIN		1.5E-45
#define BYTE_MAX	255
#define BYTE_MIN	0
#define WORD_MAX	65535
#define WORD_MIN	0
#define INTEGER_MAX	2147483647
#define INTEGER_MIN	-2147483648
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
};

int  gettoken(FILE *);
