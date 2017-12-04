/**@<parser.h>::**/

extern FILE *src;
extern int lookahead;

void match(int);
int smpExpr(int);
int expr(int);
void myPas(void);
void stmt(void);
void stmtList(void);
int isOPlus(int oplus, int);
int isOTimes(int otimes, int);
int isOTilde(int otilde);
int isNegate(void);
int operClass(int );
int typeClass (int );
