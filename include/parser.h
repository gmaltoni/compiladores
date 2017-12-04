/**@<parser.h>::**/

extern FILE *src;
extern int lookahead;

void match(int);

int smpExpr(int);

int expr(int);

void mypas(void);

void stmt(void);

void stmtlist(void);

int isoplus(int oplus, int);

int isotimes(int otimes, int);

int isotilde(int otilde);

int isnegate(void);

int operClass(int );

int typeClass (int );
