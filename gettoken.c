/**@<gettoken.c>::**/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <keywords.h>
#include <gettoken.h>

char lexeme[MAXSTRLEN+1];

char           *tokeninfo[] = {
    "ID",
    "DEC",
    "NEQ",
    "GEQ",
    "LEQ",
    "SCI",
    "FLT",
    "ASGNM",
};

void skipspaces(FILE * tape)
{
    int             head;
    while (isspace(head = getc(tape)));
    ungetc(head, tape);
}

//TODO: skipcomments

int isID(FILE * tape)
{
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
	retval = iskeyword(lexeme);
	if (retval) {
		return retval;
	} else {
	        return ID;
	}
    }

    ungetc(lexeme[i], tape);
    return 0;
}

/*int isOCT(FILE * tape)
{
    int             head;
    int i = 1;

    head = getc(tape);
    if (isdigit(head) && (head >= '1' && head <= '7')) {
	lexeme[i] = head;
	i++;
        while (isdigit(head = getc(tape)) && (head >= '1' && head <= '7')){
	    if (i < MAXSTRLEN) {
		lexeme[i] = head;
		i++;
	    }
	}
	lexeme[i] = 0;
        ungetc(head, tape);
        return OCT;
    }
    ungetc(head, tape);
    return 0;

}*/

/*int isHEX(FILE * tape)
{
    int             head;
    int             x = getc(tape);
    int i = 1;

    if (toupper(x) == 'X') {
	lexeme[i] = x;
	i++;
        head = getc(tape);
        if (isdigit(head)
            || (toupper(head) >= 'A' && toupper(head) <= 'F')) {
	    lexeme[i] = head;
	    i++;
            while (isdigit(head = getc(tape))
                   || (toupper(head) >= 'A' && toupper(head) <= 'F')){
	    	if (i < MAXSTRLEN) {
			lexeme[i] = head;
			i++;
	    	}
	    }
	    lexeme[i] = 0;
            ungetc(head, tape);
            return HEX;
        }
	i--;
	lexeme[i] = 0;
        ungetc(head, tape);
    }
    ungetc(x, tape);
    return 0;
}*/

int isdecimal(FILE * tape)
{
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

int isassignment(FILE * tape)
{
    int             head;
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

int iscomposite(FILE * tape)
{
  lexeme[2] = 0;
  switch(lexeme[0] = getc(tape))
  {
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

int isSCI(FILE * tape)
{
    int             head,
                    sgn = 0,
        expChar = 0;
    int i;
    expChar = getc(tape);

    i = strlen(lexeme);

    if (toupper(expChar) == 'E') {  // Q1 - E = 69
	lexeme[i] = expChar;
	i++;

        if ((head = getc(tape)) == '+' || (head == '-')) {  // Q2

            sgn = head;         // Q3
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

        while (isdigit(head = getc(tape))){ // Q4
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

int isnumber(FILE * tape)
{
    int             head,
                    token = 0;
    int i = 0;
    token = isdecimal(tape);

    i = strlen(lexeme);

    if (token && token != DEC)
        return token;
    else if (token == DEC) {

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
        isSCI(tape) && (token == DEC) && (token = FLT);
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
        isSCI(tape);
        return FLT;
    } else {
        ungetc(head, tape);
        return 0;
    }
}

int gettoken(FILE * tape)
{
    char teste;
    int             token;
    skipspaces(tape);
    if ((token = isID(tape)) ||
        (token = isnumber(tape)) ||
        (token = iscomposite(tape)) ||
        (token = isassignment(tape))) {
	return token;
	}
    lexeme[0] = getc(tape);
    lexeme[1] = 0;
    return lexeme[0];
}
