/**@<gettoken.c>::**/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <keywords.h>
#include <gettoken.h>

char lexeme[MAXSTRLEN+1];

char *tokeninfo[] = {
    "ID",
    "DEC",
    "NEQ",
    "GEQ",
    "LEQ",
    "SCI",
    "FLT",
    "ASGNM",
    "CH",
    "ST"
};

/*
 * Ignora espaços em branco (ex: espaço, tab, nova linha).
 */
void skipSpaces(FILE * tape) {
    int head;
    while (isspace(head = getc(tape)));
    ungetc(head, tape);
}

/*
 * Ignora comentários.
 */
void skipComments (FILE * tape) {
    int head;
    
    if ((head = getc(tape)) == '/') {
        if((head = getc(tape)) == '*') {
            _while:     while((head = getc(tape)) != '*');
            
            if ((head = getc(tape)) != '/')
                goto _while;
            
            return;
        }
        
        ungetc(head, tape);
    }
    
    ungetc(head, tape);
}

/*
 * Verifica se é um caractere ou uma cadeia de caracteres.
 */
int isString(FILE * tape) {
    int head;
    int i = 0;
    
    head = getc(tape);
    
    // Uma cadeia de caracteres sempre começa com aspas.
    if (head == '\"') {
        while(head = getc(tape) != '\"') {
            lexeme[i] = head;
            i++;
        }
        return ST;
    }
    
    // Um caractere sempre é acompanhado de apóstrofo.
    else if (head == '\'') {
        if ((lexeme[i] = getc(tape)) != '\'') {
            head = getc(tape);
            
            if (head != '\'') {
                fprintf(stderr, "fatal error: invalid character");
                //return 0;
                exit(-3);
            }
            
            return CH;
        }
        
        return CH;
    }
    
    ungetc(head, tape);
    return 0;
}

/*
 * Verifica se é uma palavra reservada ou um identificador.
 * ID = [A-Za-z][A-Za-z0-9]*
 */
int isID(FILE * tape) {
    int head;
    int i = 0, retval;
    
    if (isalpha(lexeme[i] = getc(tape))) {
	i++;
        
        while (isalnum(lexeme[i] = getc(tape))) {
	    if (i < MAXSTRLEN) {
		i++;
	    }
	}
        
        ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	retval = isKeyword(lexeme);
        
	if (retval) {
            return retval;
	} else {
            return ID;
	}
    }
    
    ungetc(lexeme[i], tape);
    return 0;
}

/*
 * Verifica se é decimal.
 * DEC = [1-9][0-9]* | 0
 */
int isDecimal(FILE * tape) {
    if(isdigit(lexeme[0] = getc(tape))) {
	int i;
	for(i = 1; isdigit(lexeme[i] = getc(tape)); (i < MAXSTRLEN) && i++);
	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return DEC;
    }
    
    ungetc(lexeme[0], tape);
    return 0;
}

/*
 * Verifica se é uma atribuição.
 * ASGNM = :=
 */
int isAssignment(FILE * tape) {
    int head;
    int i = 0;
    
    if ((head = getc(tape)) == ':') {
	lexeme[i] = head;
	i++;
        
        if ((head = getc(tape)) == '=') {
	    lexeme[i] = head;
	    i++;
	    lexeme[i] = 0;
            return ASGNM;
        }
        
        ungetc(head, tape);
        ungetc(':', tape);
        
        return 0;
    } else {
	ungetc(head, tape);
    }
    
    return 0;
}

/*
 * Verifica se é uma composto (necessita da leitura de mais um caractere).
 */
int isComposite(FILE * tape) {
    lexeme[2] = 0;
    
    switch(lexeme[0] = getc(tape)) {
        case '>':
            if ((lexeme[1] = getc(tape)) == '=') {
                lexeme[2] = 0;
                return /*>=*/GEQ;
            }
            
            break;
            
        case '<':
            if ((lexeme[1] = getc(tape)) == '>') {
                return /*<>*/NEQ;
            }
            
            if(lexeme[1] == '=') {
                return /*<=*/LEQ;
            }
            
            break;
            
        default:
            ungetc(lexeme[0], tape);
            return 0;
    }
    
    ungetc(lexeme[1], tape);
    lexeme[1] = 0;
    
    return lexeme[0];
}

/*
 * Verifica se é notação científica.
 */
int isScientificNotation(FILE * tape) {
    int head, sgn = 0, expChar = 0;
    int i;
    expChar = getc(tape);
    
    i = strlen(lexeme);
    
    if (toupper(expChar) == 'E') {
	lexeme[i] = expChar;
	i++;
        
        if ((head = getc(tape)) == '+' || (head == '-')) {
            sgn = head;
	    lexeme[i] = head;
	    i++;
            
            if (!isdigit(head = getc(tape))) {
		i = i - 2;
		lexeme[i] = 0;
                
                ungetc(head, tape);
                ungetc(sgn, tape);
                ungetc(expChar, tape);
                
                return 0;
            }
        } else if (!isdigit(head)) {
	    i--;
	    lexeme[i] = 0;
            
            ungetc(head, tape);
            ungetc(expChar, tape);
            
            return 0;
        }
        
        while (isdigit(head = getc(tape))){
	    if (i < MAXSTRLEN) {
		lexeme[i] = head;
		i++;
	    }
	}
        
	lexeme[i] = 0;
        ungetc(head, tape);
        
        return SCI;
    }
    
    else {
        ungetc(expChar, tape);
        return 0;
    }
}

/*
 * Verifica se é um número.
 * FLOAT = ( DEC ‘.’ DIGIT* | ‘.’ DIGIT+ ) EXP? | DEC EXP
 * DIGIT = [0-9]
 * EXP   =  ('E'|'e') (‘+’|‘-’)? DIGIT+
 */
int isNumber(FILE * tape) {
    int head, token = 0;
    int i = 0;
    token = isDecimal(tape);
    
    i = strlen(lexeme);
    
    if (token && token != DEC) {
        return token;
    } else if (token == DEC) {
        if ((head = getc(tape)) == '.') {
	    lexeme[i] = head;
	    i++;
            token = FLT;
            
            while (isdigit(head = getc(tape))){
	    	if (i < MAXSTRLEN) {
                    lexeme[i] = head;
                    i++;
	    	}
	    }
            
	    lexeme[i] = 0;
        }
        
        ungetc(head, tape);
        isScientificNotation(tape) && (token == DEC) && (token = FLT);
        return token;
        
    } else if ((head = getc(tape)) == '.') {
	lexeme[i] = head;
	i++;
        
        if (!isdigit(head = getc(tape))) {
            ungetc(head, tape);
            ungetc('.', tape);
            return 0;
        }
        
	lexeme[i] = head;
	i++;
        
        while (isdigit(head = getc(tape))){
	    if (i < MAXSTRLEN) {
		lexeme[i] = head;
		i++;
	    }
	}
        
	lexeme[i] = 0;
        
        ungetc(head, tape);
        isScientificNotation(tape);
        
        return FLT;
    } else {
        ungetc(head, tape);
        return 0;
    }
}

/*
 * Verifica token a token da entrada recebida.
 */
int getToken(FILE * tape) {
    int token;
    skipSpaces(tape);
    skipComments(tape);
    
    if ((token = isString(tape)) ||
        (token = isID(tape)) ||
        (token = isNumber(tape)) ||
        (token = isComposite(tape)) ||
        (token = isAssignment(tape))) {
	return token;
    }
    
    lexeme[0] = getc(tape);
    lexeme[1] = 0;
    
    return lexeme[0];
}
