/**@<dtypes.h>::**/

enum {
	OPERAND_MISMATCH = -128,
	TYPE_MISMATCH
};

enum{
INT8 = 1,
INT16,
INT32,
INT64,
FLT32,
FLT64,
BOOL,
CHR, 
STR
};

int flt2arch(char const *);
int uint2arch(char const *);
int cmptype(int , int );
