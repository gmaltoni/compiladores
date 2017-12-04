/**@<symtab.h>**/

#define MAXSYMENTRIES 0x10000

typedef struct {
	char name[MAXSTRLEN+1];
	int vtype;
} SYMTAB;

extern SYMTAB symtab[];

extern int symtab_nextentry;

int symtabLookup(char const *query);

int symtabAppend(char const *newname);

void symtabSetType(int , int);

/* not been used for while
extern double symtab_reg[];
extern int symtab_nextentry/* = 0*/;
#define MAXSTACKSIZE 0x1000
/*
extern double stack[MAXSTACKSIZE];
extern int stack_pointer/* = -1;
extern double acc/* = 0*/;

