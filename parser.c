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
    /**/int lbl_else, lbl_endif/**/;    //
    /**/lbl_else = loopcount++/**/;     //
    /**/lbl_endif = loopcount++/**/;    //
    expr(BOOL);				//		<<expr>>.as
    match(THEN);			//		jz  .L$lbl_endif
    /**/printf("\tjz  .L%d\n", lbl_else)/**/;	
    stmt();				//		<<stmt>>.as
    if (lookahead == ELSE) {            //
                                        //		jmp .L$lbl_endif
        /**/printf("\tjmp  .L%d\n", lbl_endif)/**/;
        match(ELSE);                    // .L$lbl_else:
        /**/printf(".L%d:\n", lbl_else)/**/;	
        stmt();                         //		<<stmt>>.as	
    }					//
                                        // .L$lbl_endif:
    /**/printf(".L%d:\n", lbl_endif)/**/;
}

void whlStmt() {
						// (lbl_while = lbl_endwhile = loopcount++)
    match(WHILE);				// .L$lbl_while:
    /**/int lbl_while, lbl_endwhile/**/;
    /**/lbl_while = loopcount++/**/;
    /**/lbl_endwhile = loopcount++/**/;
    /**/printf(".L%d:\n", lbl_while)/**/;
    expr(BOOL);                                 //		<<expr>>.as
    match(DO);                                  //		jz  .L$lbl_endwhile
    /**/printf("\tjz  .L%d\n", lbl_endwhile)/**/;
    stmt();					//		<<stmt>>.as
						//		jmp .L$lbl_while
    /**/printf("\tjmp  .L%d\n", lbl_while)/**/;
						// .L$lbl_endwhile:
    /**/printf(".L%d:\n", lbl_endwhile)/**/;
}

void repStmt() {
						// (lbl_repeat=lbl_endrepeat=loopcount++)
    match(REPEAT);				// .L$lbl_repeat:
    /**/int lbl_repeat, lbl_endrepeat/**/;
    /**/lbl_repeat=loopcount++/**/;
    /**/printf(".L%d:\n", lbl_repeat)/**/;
    stmtList(); 				//		<<stmtList>>.as
    match(UNTIL);				
						//		jnz .L$lbl_repeat
    expr(BOOL); 				//		<<expr>>.as
    /**/printf("\tjnz  .L%d\n", lbl_repeat)/**/;
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

    /**/int otilde, oprnd1, oprnd2/**/;

    /**/oprnd1 = /**/smpExpr(0);

    if (isOTilde(/**/otilde =/**/ lookahead)) {
        match(otilde);

        /**/oprnd2 =/**/ smpExpr(/**/oprnd1/**/);

        /** Início da checagem de tipos **/
        if(typeClass(oprnd1) != typeClass(oprnd2)) {
            fprintf(stderr, "fatal error: incompatible operation %d with %d.\n", oprnd1, oprnd2);
            return -1;
        } else {
            oprnd1 = BOOL;
        }
    }

    if (inheritedType == BOOL && oprnd1 != BOOL) {
        fprintf(stderr, "incompatible operation %d with %d: fatal error.\n", inheritedType, oprnd1);
        return -1;
    }
    
    return oprnd1;
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
  /**/int sign, otimes = 0, oplus = 0, acctype = 0, R_type, L_type, valSeen = 0/**/;
  /**/char name[MAXSTRLEN+1]/**/;

E_start:
    /**/sign = /**/isNegate();
    if(sign) match(sign);
T_start:
F_start:
    /*1* if(oplus + otimes){
	    	acc_push();
	 } /*1*/
    switch (lookahead) {
        case ID:
            /*2*/strcpy(name, lexeme)/*2*/;

            /**/int symtab_descriptor = symtabLookup(name)/**/;
            /**/if (symtab_descriptor) {
                L_type = symtab[symtab_descriptor].vtype;
            } else {
                fprintf(stderr, "left-assignment variable \"%s\" not declared\n", lexeme);
                L_type = -1;
            }/**/

            if (acctype == 0){
                acctype = L_type;
            }

            match(ID);
            if (lookahead == ASGNM) {

                /** ASGNM = ":=" **/
                match(ASGNM);
                R_type = smpExpr(/**/inheritedType/**/);

                /**type compatibility check regarding L-typ and R-type **/
                if (isCompatible(L_type, R_type)) {
                    acctype = max(R_type = L_type, acctype);
                } else {
                    fprintf(stderr, "error: R-type mismatch L-type\n");
                    acctype = TYPE_MISMATCH;
                }

                valSeen = 1;

            } else {
                    /**/
                    // vd is the variable descriptor in the symbol table
                    int vd = symtabLookup(name);
                    if (vd > 0) {
                        R_type = symtab[vd].vtype;
                    } else {
                        fprintf(stderr, "error: undeclared variable\"%s\"\n", name);
                    }
                    /**/
                    switch(R_type) {
                        case BYTE:
                            printf("\tpushb %%al\n\tmovb %s,%%al\n",symtab[vd].name);
                            break;
                        case WORD:
                            printf("\tpushw %%ax\n\tmovw %s,%%ax\n",symtab[vd].name);
                            break;
                        case INTEGER:
                            printf("\tpushl %%eax\n\tmovl %s,%%eax\n",symtab[vd].name);
                            break;
                        case LONGINT:
                            printf("\tpushq %%rax\n\tmovq %s,%%rax\n",symtab[vd].name);
                            break;
                        case REAL:
                            printf("\tpushss %%xmn0\n\tmovss %s,%%xmn0\n",symtab[vd].name);
                            break;
                        case DOUBLE:
                            printf("\tpushsd %%xmn0\n\tmovsd %s,%%xmn0\n",symtab[vd].name);
                            break;
                    }/**/
            }
            break;

        case FLT:

            /**/ R_type = flt2arch(lexeme, sign)/**/;
            /**/rmove(lexeme, R_type)/**/;
            match(FLT);
            break;
        case DEC:

            /**/ R_type = uint2arch(lexeme, sign)/**/;
            /**/rmove(lexeme, R_type)/**/;
            match(DEC);
            break;
        case CH:
            /**/ R_type = CHAR/**/;
            match(CH);
            break;
        case ST:
            /**/ R_type = STRING/**/;
            match(ST);
            break;
        case TRUE:
            R_type = BOOLEAN;
            match(TRUE);
            break;
        case FALSE:
            R_type = BOOLEAN;
            match(FALSE);
            break;
        default:
            match('('); /**/R_type = /**/smpExpr(0); match(')');
    }

    /**/
    if (acctype == 0) { 
        acctype = R_type;
    } else {
	if (typeClass(acctype) == typeClass(R_type)) {
		acctype = max(acctype, R_type);
	} else {
            fprintf(stderr, "error: type mismatch\n");
            acctype = TYPE_MISMATCH;
	}
   }/**/

    /*12*if (otimes){calculate(otimes);otimes=0;}/*12*/

F_end:
    if (/**/otimes = /**/isOTimes(lookahead, acctype)) {
	/**/
	//TODO: verificar mod e div com inteiros
	if (typeClass(acctype) != operClass(otimes)) {
            fprintf(stderr, "error: operato type does not match operand type\n");
            acctype = OPERAND_MISMATCH;
	}
	/**/
        match(lookahead);
        goto F_start;
    }

T_end:
    /*11*/if (sign) {
	chs(acctype);
	sign = 0;
    }/*11*/

    /*13*if (oplus) {calculate(oplus);oplus=0;}/*13*/

    if (/**/oplus = /**/isOPlus(lookahead, acctype)) {
	/**/
	if (typeClass(acctype) != operClass(oplus)) {
            fprintf(stderr, "error: operato type does not match operand type\n");
            acctype = OPERAND_MISMATCH;
	}
	/**/
        match(lookahead);
        goto T_start;
    }

    if (valSeen == 1) {
	lmove(name,acctype);
    }

E_end:
    ;
    return acctype;
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
            /**/asmAdd(type)/**/;
            return '+';
        case '-':
            /**/asmSub(type)/**/;
            return '-';
        case OR:
            /**/asmMulLog()/**/;
            return OR;
    }

    return 0;
}

int isOTimes(int otimes, int type)
{
    switch (otimes) {
        case '*':
            /**/asmMul(type)/**/;
            return '*';
        case '/':
            /**/asmDiv(type)/**/;
            return '/';
        case DIV:
            return DIV;
        case AND:
            /**/asmAddLog()/**/;
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
    /*16*/int a = symtab_nextentry/*16*/;
    
VAR_LIST:
    /*17*/if (symtabAppend(lexeme) == 0) {
            fprintf(stderr, "fatal error: %s already declared\n", lexeme);	
    }/*17*/

    match(ID);
    
    if (lookahead == ',') {
        match(',');
        goto VAR_LIST;
    }
    
    return a;
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
    /**/int range_start, range_type/**/;
    /* vardec1 --> VAR varList : varRightType ; */
    /* varList --> ID { , ID } 
     * varRightType --> BYTE | WORD | INTEGER | LONGINT | REAL | DOUBLE |
     *                  BOOLEAN | CHAR | STRING
     */

    if (lookahead == VAR)  {
	match(VAR);
WHLID:
	range_start = varList();
	match(':');
	range_type = varRightType();
        match(';');
        
	/**/symtabSetType(range_type,range_start)/**/;
        
	if(lookahead==ID) goto WHLID;
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
