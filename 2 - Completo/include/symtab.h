/**@<symtab.h>**/

#define MAXSYMENTRIES 0x10000
#define MAXSTACKSIZE 0x1000

typedef struct {
    char name[MAXSTRLEN+1];
    int vtype;
} SYMTAB;

extern SYMTAB symtab[];
extern int symtab_nextentry;

int symtabLookup(char const *query);
int symtabAppend(char const *newname);
void symtabSetType(int , int);