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
     		case INTEGER:
			switch(rtype) {
	       			case INTEGER:
				case WORD:
				case BYTE:
					return ltype;
			}

     		case REAL:
       			switch(rtype) {
         			case INTEGER:
				case WORD:
				case BYTE:
         			case REAL:
         				return ltype;
       			}
       			break;

     		case DOUBLE:
       			switch(rtype) {
         			case INTEGER:
				case WORD:
				case BYTE:
         			case REAL:
         			case DOUBLE:
           				return ltype;
       			}
		case STRING:
			switch(rtype) {
				case STRING:
				case CHAR:
					return ltype;
			}
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
  /**/int sign, otimes = 0, oplus = 0, acctype = 0, R_type, L_type/**/;
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
	}
        break;

    case FLT:

	/**/ R_type = flt2arch(lexeme)/**/;

	match(FLT);
	break;
    case DEC:

	/**/ R_type = uint2arch(lexeme)/**/;

	match(DEC);
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
    if (/**/otimes = /**/isotimes(lookahead)) {
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
    /*11*if (sign) {
	chs();
	sign = 0;
    }/*11*/

    /*13*if (oplus) {calculate(oplus);oplus=0;}/*13*/

    if (/**/oplus = /**/isoplus(lookahead)) {
	/**/
	if (typeclass(acctype) != operclass(oplus)) {
		fprintf(stderr, "error: operato type does not match operand type\n");
		acctype = OPERAND_MISMATCH;
	}
	/**/
        match(lookahead);
        goto T_start;
    }

  E_end:
    ;
    return acctype;
}

/** Initial grammar symbol: mypas **
 *
 * mypas -> { stmt } EOF
 *
 * stmt -> [ E ];
 *
 * */

int isFIRST_E(void)
{
    switch (lookahead) {
    case '(':
    case ID:
    case DEC:
    case FLT:
        return lookahead;
    case ';':
        return 0;
    default:
        return -1;
    }
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

int isoplus(int oplus)
{
    switch (oplus) {
    case '+':
	return '+';
    case '-':
        return '-';
    }

    return 0;
}

int isotimes(int otimes)
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
	/**/int ltype, rtype, symtab_descriptor/**/;
    	switch(lookahead){
		case BEGIN:  match(BEGIN);
			     stmtlist();
			     match(END);
			     break;
		case IF:     match(IF);
			     expr(BOOL);
			     match(THEN);
			     stmt();
			     if (lookahead==ELSE) {match(ELSE);stmt();}
			     break;
		case WHILE:  match(WHILE);
			     expr(BOOL);
			     match(DO);
			     stmt();
			     break;
		case REPEAT: match(REPEAT);
			     stmtlist();
			     match(UNTIL);
			     expr(BOOL);
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

int flt2arch(char const * name) {
	double val;
	val = strtod(name, NULL);	
	if (val > FLT_MAX || val < FLT_MIN)
		return DOUBLE;
	return REAL;
}

int uint2arch(char const * name) {
	long val;
	val = atol(name);
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


/*
 * addop -> '+' | '-' | "||" | == | != ...
 */
/*
 * mulop -> '*' | '/' | && | %
 */
/*
 * asgnm -> '='
 */
/*
 * var   -> ID
 */
/*
 * ct    -> DEC
 */
