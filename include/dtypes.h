/**@<dtypes.h>::**/

#define FLT_MAX		3.4E38
#define FLT_MIN		1.5E-45
#define BYTE_MAX	255
#define BYTE_MIN	0
#define WORD_MAX	65535
#define WORD_MIN	0
#define INTEGER_MAX	2147483647
#define INTEGER_MIN	-2147483648

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

int flt2arch(char const *, int);
int uint2arch(char const *, int);
int cmptype(int , int );
