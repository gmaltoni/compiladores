/**@<parser.h>::**/

extern FILE *src;
extern int lookahead;

void match(int);

int smpexpr(int);

int expr(int);

void mypas(void);

void stmt(void);

void stmtlist(void);

int isoplus(int oplus);

int isotimes(int otimes);

int isotilde(int otilde);

int isnegate(void);

int operclass(int );

int typeclass (int );
