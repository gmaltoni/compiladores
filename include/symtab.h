/**@<symtab.h>**/

#define MAXSYMENTRIES 0x10000

typedef struct {
	char name[MAXSTRLEN+1];
	int vtype;
} SYMTAB;

extern SYMTAB symtab[];

extern int symtab_nextentry;

int symtab_lookup(char const *query);

int symtab_append(char const *newname);

void symtab_settype(int , int);

/* not been used for while
extern double symtab_reg[];
extern int symtab_nextentry/* = 0*/;
#define MAXSTACKSIZE 0x1000
/*
extern double stack[MAXSTACKSIZE];
extern int stack_pointer/* = -1;
extern double acc/* = 0*/;

