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

/** Imperative syntax **/
/**/int loopcount = 0;
void ifstmt() {
	match(IF);				// (lbl_else=lbl_endif=loopcount++)
	/**/int lbl_else, lbl_endif/**/;
	/**/lbl_else=loopcount++/**/;
	/**/lbl_endif=loopcount++/**/;
	expr(BOOL);				//		<<expr>>.as
	match(THEN);				//		jz  .L$lbl_endif
	/**/printf("\tjz  .L%d\n", lbl_else)/**/;	
	stmt();					//		<<stmt>>.as
	if (lookahead==ELSE) 	
	{		
						//		jmp .L$lbl_endif
		/**/printf("\tjmp  .L%d\n", lbl_endif)/**/;
		match(ELSE);			// .L$lbl_else:
		/**/printf(".L%d:\n", lbl_else)/**/;	
		stmt();				//		<<stmt>>.as	
	}					//
						// .L$lbl_endif:
	/**/printf(".L%d:\n", lbl_endif)/**/;
}

void whlstmt() {
						// (lbl_while=lbl_endwhile=loopcount++)
	match(WHILE);				// .L$lbl_while:
	/**/int lbl_while, lbl_endwhile/**/;
	/**/lbl_while=loopcount++/**/;
	/**/lbl_endwhile=loopcount++/**/;
	/**/printf(".L%d:\n", lbl_while)/**/;
	expr(BOOL);				//		<<expr>>.as
	match(DO);				//		jz  .L$lbl_endwhile
	/**/printf("\tjz  .L%d\n", lbl_endwhile)/**/;
	stmt();					//		<<stmt>>.as
						//		jmp .L$lbl_while
	/**/printf("\tjmp  .L%d\n", lbl_while)/**/;
						// .L$lbl_endwhile:
	/**/printf(".L%d:\n", lbl_endwhile)/**/;
}

void repstmt() {
						// (lbl_repeat=lbl_endrepeat=loopcount++)
	match(REPEAT);				// .L$lbl_repeat:
	/**/int lbl_repeat, lbl_endrepeat/**/;
	/**/lbl_repeat=loopcount++/**/;
	/**/printf(".L%d:\n", lbl_repeat)/**/;
	stmtlist();				//		<<stmtlist>>.as
	match(UNTIL);				
						//		jnz .L$lbl_repeat
	expr(BOOL);				//		<<expr>>.as
	/**/printf("\tjnz  .L%d\n", lbl_repeat)/**/;
}

void bgnstmt() {
	match(BEGIN);
	stmtlist();
	match(END);
}

/** Syntax definition **/

/*
 * Proto-Pascal syntax -- LL(1) EBNF
 */

/* expr --> E [ relop E ] */
/* typecheck implemented*/
int expr(int inherited_type) {

	/**/int otilde, oprnd1, oprnd2/**/;

    	/**/oprnd1 = /**/smpexpr(0);

    	if (isotilde(/**/otilde =/**/ lookahead)) {
        	match(otilde);

		/**/oprnd2 =/**/ smpexpr(/**/oprnd1/**/);

		/** typecheck begin **/
		if(typeclass(oprnd1) != typeclass(oprnd2)) {
			
			fprintf(stderr, "fatal error: incompatible operation %d with %d.\n",oprnd1,oprnd2);
			return -1;
		} else {
			oprnd1 = BOOL;
		}
    	}

    	if(inherited_type == BOOL && oprnd1 != BOOL){
    		fprintf(stderr, "incompatible operation %d with %d: fatal error.\n",inherited_type,oprnd1);
       		return -1;
  	}
  	return oprnd1;
    
}

int iscompatible(int ltype, int rtype)
{
	switch(ltype) {

     		case BOOLEAN:
		case BYTE:
		case CHAR:
			if(rtype == ltype)
         		return ltype;
       			break;
		case WORD:
			switch(rtype) {
				case BYTE:
				case WORD:
					return ltype;
			}
			break;
     		case INTEGER:
			switch(rtype) {
	       			case INTEGER:
				case WORD:
				case BYTE:
					return ltype;
			}
			break;
		case LONGINT:
			switch(rtype) {
				case LONGINT:
	       			case INTEGER:
				case WORD:
				case BYTE:
					return ltype;
			}
			break;
     		case REAL:
       			switch(rtype) {
				case LONGINT:
         			case INTEGER:
				case WORD:
				case BYTE:
         			case REAL:
         				return ltype;
       			}
       			break;

     		case DOUBLE:
       			switch(rtype) {
				case LONGINT:
         			case INTEGER:
				case WORD:
				case BYTE:
         			case REAL:
         			case DOUBLE:
           				return ltype;
       			}
			break;
		case STRING:
			switch(rtype) {
				case STRING:
				case CHAR:
					return ltype;
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

int typeclass (int type) {
	if (type>= BYTE && type<= DOUBLE) 
		return NUMERIC;
	else if (type == BOOL)
		return LOGICAL;
	return LITERAL;
}


int smpexpr(int inherited_type)
{
  /**/int sign, otimes = 0, oplus = 0, acctype = 0, R_type, L_type, val_seen = 0/**/;
  /**/char name[MAXSTRLEN+1]/**/;

  E_start:
	/**/sign = /**/isnegate();
	if(sign) match(sign);
  T_start:
  F_start:
    /*1* if(oplus + otimes){
	    	acc_push();
	}/*1*/
    switch (lookahead) {
    case ID:
	/*2*/strcpy(name, lexeme)/*2*/;

	/**/int symtab_descriptor = symtab_lookup(name)/**/;
	/**/if (symtab_descriptor) {
		L_type = symtab[symtab_descriptor].vtype;
	}else {
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
            R_type = smpexpr(/**/inherited_type/**/);

	    /**type compatibility check regarding L-typ and R-type **/
	    if (iscompatible(L_type, R_type)) {
		acctype = max(R_type = L_type, acctype);
	    } else {
		fprintf(stderr, "error: R-type mismatch L-type\n");
		acctype = TYPE_MISMATCH;
	    }
	    val_seen = 1;

        } else {
	    	/**/
		// vd is the variable descriptor in the symbol table
		int vd = symtab_lookup(name);
	    	if (vd > 0) {
			R_type = symtab[vd].vtype;
		} else {
			fprintf(stderr, "error: undeclared variable\"%s\"\n", name);
		}
		/**/
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
        match('('); /**/R_type = /**/smpexpr(0); match(')');
    }

   /**/
   if (acctype == 0) { 
	acctype = R_type;
   } else {
	if (typeclass(acctype) == typeclass(R_type)) {
		acctype = max(acctype, R_type);
	} else {
		fprintf(stderr, "error: type mismatch\n");
		acctype = TYPE_MISMATCH;
	}
   }/**/

    /*12*if (otimes){calculate(otimes);otimes=0;}/*12*/

  F_end:
    if (/**/otimes = /**/isotimes(lookahead, acctype)) {
	/**/
	//TODO: verificar mod e div com inteiros
	if (typeclass(acctype) != operclass(otimes)) {
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

    if (/**/oplus = /**/isoplus(lookahead, acctype)) {
	/**/
	if (typeclass(acctype) != operclass(oplus)) {
		fprintf(stderr, "error: operato type does not match operand type\n");
		acctype = OPERAND_MISMATCH;
	}
	/**/
        match(lookahead);
        goto T_start;
    }
    if (val_seen == 1) {
	lmove(name,acctype);
    }

  E_end:
    ;
    return acctype;
}

int operclass(int oper) {
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

int isoplus(int oplus, int type)
{
    switch (oplus) {
    case '+':
	/**/asm_add(type)/**/;
	return '+';
    case '-':
	/**/asm_sub(type)/**/;
        return '-';
    case OR:
	/**/asm_mullog()/**/;
	return OR;
    }

    return 0;
}

int isotimes(int otimes, int type)
{
    switch (otimes) {
    case '*':
	/**/asm_mul(type)/**/;
	return '*';
    case '/':
	/**/asm_div(type)/**/;
        return '/';
    case DIV:
	return DIV;
    case AND:
	/**/asm_addlog()/**/;
	return AND;
    case MOD:
	return MOD;
    }

    return 0;
}

int isotilde(int otilde) {
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

int varlist(void) {	
	/*16*/int a = symtab_nextentry/*16*/;
    VAR_LIST:
	/*17*/if (symtab_append(lexeme) == 0) {
		fprintf(stderr, "fatal error: %s already declared\n", lexeme);	
	}/*17*/
        match(ID);
        if (lookahead == ',') {
	    match(',');
	    goto VAR_LIST;
	}
	return a;
}

int vartype(void) {
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
    /* vardec1 --> VAR varlist : vartype ; */
    /* varlist --> ID { , ID } 
     * vartype --> BYTE | WORD | INTEGER | LONGINT | REAL | DOUBLE
     *           | BOOLEAN | CHAR | STRING
     */

    if (lookahead == VAR)  {
	match(VAR);
WHLID:
	range_start = varlist();
	match(':');
	range_type = vartype();
        match(';');
	/**/symtab_settype(range_type,range_start)/**/;
	if(lookahead==ID) goto WHLID;
    }
}



void stmt(void)
{
    	switch(lookahead){
		case BEGIN:  bgnstmt();
			     break;
		case IF:     ifstmt();
			     break;
		case WHILE:  whlstmt();
			     break;
		case REPEAT: repstmt();
			     break;

		 case ID: //tokens.h
		 case FLT: //tokens.h
		 case DEC: //tokens.h
		 case TRUE: //keywords.h
		 case FALSE: //keywords.h
		 case NOT: //keywords.h
		 case '-':
		 case '(':
		      	smpexpr(0);
		      	break;
		default:     ;
	}
}

void stmtlist(void) {
    STMT_LIST:
        stmt();
        if (lookahead == ';') {
	    match(';');
            goto STMT_LIST;   
	}
}

void imperative(void) {
    match(BEGIN);
    stmtlist();
    match(END);
    match('.');
}

void mypas(void) {
    declarative();
    imperative();
}

/** lexer-to-parser interface **/

int lookahead;

void match(int expected) {
    if (expected == lookahead) {
        lookahead = gettoken(src);
    } else {
        fprintf(stderr, "token mismatch\n");
        fprintf(stderr, "expected '%c' ", expected);
        fprintf(stderr, "whereas found %d\n", lookahead);
        exit(-2);
    }
}

int isnegate() {
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
	if (sign) val = -val;	
	if (val > FLT_MAX || val < FLT_MIN)
		return DOUBLE;
	return REAL;
}

int uint2arch(char const * name, int sign) {
	long val;
	val = atol(name);
	if (sign) val = -val;
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
