#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <gettoken.h>
#include <myasm.h>
#include <string.h>

/** auxiliary memory stuff **
double stack[MAXSTACKSIZE];
int stack_pointer = -1;
double acc = 0;

/** operations**/
void chs(void) {
	//acc = -acc;
}

void acc_push(void) {
	/*if (stack_pointer == MAXSTACKSIZE) {
		fprintf(stderr, "erro estouro da pilha\n");
		exit(-4);
	}
	stack_pointer++;
	stack[stack_pointer] = acc;*/
}

void acc_store(char const *name) {
	/*int address = symtab_lookup(name);
	if (address == -1) {
		if (symtab_nextentry < MAXSYMENTRIES) {
			address = symtab_nextentry;
			symtab_nextentry++;
			strcpy(symtab[address], name);
		} else {
			fprintf(stderr, "erro estouro da tabela\n");
			exit(-3);
		}
	}
	symtab_reg[address] = acc;*/
}

void acc_recall(char const *name) {
	/*int address = symtab_lookup(name);
	if (address > -1) {
		acc = symtab_reg[address];
	} else {
		acc = 0;
	}*/	
}

void acc_immediate(double val) {
	/*acc = val;*/
}

void calculate(int operador) {
    /*switch (operador) {
    case '+':
	acc = stack[stack_pointer] + acc; 
	break;
    case '-':
	acc = stack[stack_pointer] - acc; 
        break;
    case '*':
	acc = stack[stack_pointer] * acc; 
	break;
    case '/':
	acc = stack[stack_pointer] / acc; 
        break;
    }
    stack_pointer--;*/
}
