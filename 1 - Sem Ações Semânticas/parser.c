/**@<parser.c>::**/

#include <stdlib.h>
#include <stdio.h>
#include <gettoken.h>
#include <parser.h>
#include <string.h>
#include <myasm.h>
#include <dtypes.h>
#include <keywords.h>
#include <macros.h>
#include <symtab.h>

/** Sintaxe imperativa. **/

/**/int loopcount = 0;

void ifStmt() {
    match(IF);				// (lbl_else = lbl_endif=loopcount++)
    expr(BOOL);				//		<<expr>>.as
    match(THEN);			//		jz  .L$lbl_endif
    stmt();				//		<<stmt>>.as
    if (lookahead == ELSE) {            //
                                        //		jmp .L$lbl_endif
        match(ELSE);                    // .L$lbl_else:
        stmt();                         //		<<stmt>>.as
    }					//
                                        // .L$lbl_endif:
}

void whlStmt() {
                                        // (lbl_while = lbl_endwhile = loopcount++)
    match(WHILE);			// .L$lbl_while:
    expr(BOOL);                         //		<<expr>>.as
    match(DO);                          //		jz  .L$lbl_endwhile
    stmt();				//		<<stmt>>.as
                                        //		jmp .L$lbl_while
                                        // .L$lbl_endwhile:
}

void repStmt() {
                                        // (lbl_repeat=lbl_endrepeat=loopcount++)
    match(REPEAT);			// .L$lbl_repeat:
    stmtList(); 			//		<<stmtList>>.as
    match(UNTIL);
                                        //		jnz .L$lbl_repeat
    expr(BOOL);                         //		<<expr>>.as
}

void bgnStmt() {
    match(BEGIN);
    stmtList();
    match(END);
}

/** Definição da sintaxe **/

/*
 * Sintaxe do Proto-Pascal -- LL(1) EBNF
 */

/* expr --> E [ relop E ] */
int expr(int inheritedType) {
    if (isOTilde(lookahead)) {
        match(lookahead);
    }
    
    return 0;
}

/*
 * Verifica a compatibilidade de tipos.
 */
int isCompatible(int leftType, int rightType) {
    switch(leftType) {
        case BOOLEAN:
        case BYTE:
        case CHAR:
            if(rightType == leftType)
                return leftType;
            break;
        case WORD:
            switch(rightType) {
                case BYTE:
                case WORD:
                    return leftType;
            }
            break;
        case INTEGER:
            switch(rightType) {
                case INTEGER:
                case WORD:
                case BYTE:
                    return leftType;
            }
            break;
        case LONGINT:
            switch(rightType) {
                case LONGINT:
                case INTEGER:
                case WORD:
                case BYTE:
                    return leftType;
            }
            break;
        case REAL:
            switch(rightType) {
                case LONGINT:
                case INTEGER:
                case WORD:
                case BYTE:
                case REAL:
                    return leftType;
            }
            break;
        case DOUBLE:
            switch(rightType) {
                case LONGINT:
                case INTEGER:
                case WORD:
                case BYTE:
                case REAL:
                case DOUBLE:
                    return leftType;
            }
            break;
        case STRING:
            switch(rightType) {
                case STRING:
                case CHAR:
                    return leftType;
            }
            break;
        default:
            return 0;
            break;
    }
    
    return 0;
}

enum {
    NUMERIC = 1,
    INTONLY,
    LOGICAL,
    LITERAL
};

/*
 * Define a classificação da tipagem.
 */
int typeClass(int type) {
    if (type >= BYTE && type <= DOUBLE)
        return NUMERIC;
    else if (type == BOOL)
        return LOGICAL;
    return LITERAL;
}

int smpExpr(int inheritedType) {
    printf("aqui1");
    E_start:
            if(isNegate())
                match(lookahead);
    T_start:
            F_start:
            switch (lookahead) {
                case ID:
                    match(ID);
                    if (lookahead == ASGNM) {
                        /** ASGNM = ":=" **/
                        match(ASGNM);
                        smpExpr(0);
                    }
                    printf("\n %d \n", lookahead);
                    break;
                    
                case FLT:
                    match(FLT);
                    break;
                case DEC:
                    match(DEC);
                    break;
                case CH:
                    match(CH);
                    break;
                case ST:
                    match(ST);
                    break;
                case TRUE:
                    match(TRUE);
                    break;
                case FALSE:
                    match(FALSE);
                    break;
                default:
                    match('(');
                    smpExpr(0);
                    match(')');
            }
            
            F_end:
                    if (isOTimes(lookahead, 0)) {
                        match(lookahead);
                        goto F_start;
                    }
            
            T_end:
                    if (isOPlus(lookahead, 0)) {
                        match(lookahead);
                        goto T_start;
                    }
            
            E_end:
                    ;
            return 0;
}

/*
 * Define a classificação da operação.
 */
int operClass(int oper) {
    switch(oper) {
        case '+':
        case '-':
        case '*':
        case '/':
            return NUMERIC;
        case DIV:
        case MOD:
            return INTONLY;
        case AND:
        case OR:
        case NOT:
            BOOL;
    }
    
    return 0;
}

int isOPlus(int oplus, int type)
{
    switch (oplus) {
        case '+':
            return '+';
        case '-':
            return '-';
        case OR:
            return OR;
    }
    
    return 0;
}

int isOTimes(int otimes, int type)
{
    switch (otimes) {
        case '*':
            return '*';
        case '/':
            return '/';
        case DIV:
            return DIV;
        case AND:
            return AND;
        case MOD:
            return MOD;
    }
    
    return 0;
}

int isOTilde(int otilde) {
    switch(otilde) {
        case '=':
        case NEQ:
      	case GEQ:
      	case LEQ:
      	case '>':
      	case '<':
            return otilde;
    }
    
    return 0;
}

int varList(void) {
    VAR_LIST:
            match(ID);
    
    if (lookahead == ',') {
        match(',');
        goto VAR_LIST;
    }
    
    return 0;
}

int varRightType(void) {
    int type = lookahead;
    
    switch(lookahead) {
        case BYTE:
        case WORD:
        case INTEGER:
        case LONGINT:
        case REAL:
        case DOUBLE:
        case BOOLEAN:
        case CHAR:
            match(lookahead);
            break;
        default:
	    match(STRING);
    }
    
    return type;
}

void declarative(void) {
    /* vardec1 --> VAR varList : varRightType ; */
    /* varList --> ID { , ID }
     * varRightType --> BYTE | WORD | INTEGER | LONGINT | REAL | DOUBLE |
     *                  BOOLEAN | CHAR | STRING
     */
    
    if (lookahead == VAR)  {
        match(VAR);
        WHLID:
            varList();
      	match(':');
      	varRightType();
        match(';');
        
        if(lookahead==ID)
            goto WHLID;
    }
}

void stmt(void) {
    switch(lookahead) {
        case BEGIN:
            bgnStmt();
            break;
        case IF:
            ifStmt();
            break;
        case WHILE:
            whlStmt();
            break;
        case REPEAT:
            repStmt();
            break;
        case ID:    //tokens.h
        case FLT:   //tokens.h
        case DEC:   //tokens.h
        case TRUE:  //keywords.h
        case FALSE: //keywords.h
        case NOT:   //keywords.h
        case '-':
        case '(':
            smpExpr(0);
            break;
        default:
            ;
    }
}

void stmtList(void) {
    STMT_LIST:
            stmt();
    if (lookahead == ';') {
        match(';');
        goto STMT_LIST;
    }
}

void imperative(void) {
    match(BEGIN);
    stmtList();
    match(END);
    match('.');
}

void myPas(void) {
    declarative(); // Símbolos serão declarados aqui.
    imperative(); // Símbolos serão usados aqui.
}

/** Interface lexer-to-parser **/

int lookahead;

void match(int expected) {
    if (expected == lookahead) {
        lookahead = getToken(src);
    } else {
        fprintf(stderr, "token mismatch\n");
        fprintf(stderr, "expected '%c' ", expected);
        fprintf(stderr, "whereas found %d\n", lookahead);
        exit(-2);
    }
}

int isNegate() {
    switch (lookahead){
        case '-':
        case NOT:
            return lookahead;
        default:
            return 0;
    }
}

int flt2arch(char const * name, int sign) {
    double val;
    val = strtod(name, NULL);
    
    if (sign)
        val = -val;
    
    if (val > FLT_MAX || val < FLT_MIN)
        return DOUBLE;
    
    return REAL;
}

int uint2arch(char const * name, int sign) {
    long val;
    val = atol(name);
    
    if (sign)
        val = -val;
    
    if (val > BYTE_MAX || val < BYTE_MIN) {
        if (val > WORD_MAX || val < WORD_MIN) {
            if (val > INTEGER_MAX || val < INTEGER_MIN) {
                return LONGINT;
            }
            
            return INTEGER;
        }
        
        return WORD;
    }
    
    return BYTE;
}
