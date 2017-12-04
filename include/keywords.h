/**@<keywords.h>::**/

enum {
    BEGIN = 2048,
    VAR,
    BYTE	/*INT8  - 1*/,
    WORD	/*INT16 - 2*/,
    INTEGER	/*INT32 - 3*/,
    LONGINT	/*INT64 - 4*/,
    REAL	/*FLT32 - 5*/,
    DOUBLE	/*FLT64 - 6*/,
    BOOLEAN	/*BOOL  - 7*/,
    CHAR	/*CHR   - 8*/, 
    STRING	/*STR   - 9*/,
    IF,
    THEN,
    ELSE,
    WHILE,
    DO,
    REPEAT,
    UNTIL,
    NOT,
    DIV,
    MOD,
    AND,
    OR,
    TRUE,
    FALSE,
    END
};

int isKeyword(char const *identifier);
