/**@<myasm.c>::**/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <gettoken.h>
#include <myasm.h>
#include <string.h>
#include <keywords.h>

/*
 * Adição binária.
 */
void asmAdd(int type) {
    switch(type) {
        case BYTE:
            printf("\taddb (%%bh),%%al\n");
            break;
	case WORD:
            printf("\taddw (%%sp),%%ax\n");
            break;
	case INTEGER:
            printf("\taddl (%%esp),%%eax\n");
            break;
	case LONGINT:
            printf("\taddq (%%rsp),%%rax\n");
            break;
	case REAL:
            printf("\taddss (%%xmn1),%%xmn0\n");
            break;
	case DOUBLE:
            printf("\taddsd (%%xmn1),%%xmn0\n");
            break;
    }
}

/*
 * Subtração binária.
 */
void asmSub(int type) {
    switch(type) {
	case BYTE:
            printf("\tsubb (%%bh),%%al\n");
            break;
	case WORD:
            printf("\tsubw (%%sp),%%ax\n");
            break;
	case INTEGER:
            printf("\tsubl (%%esp),%%eax\n");
            break;
	case LONGINT:
            printf("\tsubq (%%rsp),%%rax\n");
            break;
	case REAL:
            printf("\tsubss (%%xmn1),%%xmn0\n");
            break;
	case DOUBLE:
            printf("\tsubsd (%%xmn1),%%xmn0\n");
            break;
    }
}

/*
 * Multiplicação binária.
 */
void asmMul(int type) {
    switch(type) {
	case BYTE:
            printf("\tmulb (%%bh),%%al\n");
            break;
	case WORD:
            printf("\tmulw (%%sp),%%ax\n");
            break;
	case INTEGER:
            printf("\tmull (%%esp),%%eax\n");
            break;
	case LONGINT:
            printf("\tmulq (%%rsp),%%rax\n");
            break;
	case REAL:
            printf("\tmulss (%%xmn1),%%xmn0\n");
            break;
	case DOUBLE:
            printf("\tmulsd (%%xmn1),%%xmn0\n");
            break;
    }
}

/*
 * Divisão binária.
 */
void asmDiv(int type) {
    switch(type) {
	case BYTE:
            printf("\tdivb (%%bh),%%al\n");
            break;
	case WORD:
            printf("\tdivw (%%sp),%%ax\n");
            break;
	case INTEGER:
            printf("\tdivl (%%esp),%%eax\n");
            break;
	case LONGINT:
            printf("\tdivq (%%rsp),%%rax\n");
            break;
	case REAL:
            printf("\tdivss (%%xmn1),%%xmn0\n");
            break;
	case DOUBLE:
            printf("\tdivsd (%%xmn1),%%xmn0\n");
            break;
    }
}

void asmAddLog(void) {
    printf("\tor %%eax, (%%esp)\n");
    printf("\tpopl %%eax\n");
}

/*
 * Multiplicação lógica.
 */
void asmMulLog(void) {
    printf("\tand %%eax, (%%esp)\n");
    printf("\tpopl %%eax\n");
}

/** Operações **/

/*
 * Nega um valor.
 */
void chs(int type) {
    switch(type) {
	case BYTE:
            printf("\tnegateb %%al\n");
            break;
	case WORD:
            printf("\tnegatew %%ax\n");
            break;
	case INTEGER:
            printf("\tnegatel %%eax\n");
            break;
	case LONGINT:
            printf("\tnegateq %%rax\n");
            break;
	case REAL:
            printf("\tnegatess %%xmn0\n");
            break;
	case DOUBLE:
            printf("\tnegatesd %%xmn0\n");
            break;
    }
}

/*
 * jz = Jump if zero.
 */
void goFalse(int label) {
    printf("\tjz .L%d\n", label);
}

/*
 * jmp = Unconditional Jumps.
 */
void jump (int label) {
    printf("\tjmp .L%d\n", label);
}

/*
 * jle = Jump if Less or Equal.
 */
void jle(int label) {
    printf("\tjle .L%d\n", label);
}

void jlt(int label) {
    printf("\tjlt .L%d\n", label);
}

/*
 * jge = Jump if Greater or Equal.
 */
void jge(int label) {
    printf("\tjge .L%d\n", label);
}

void jgt(int label) {
    printf("\tjgt .L%d\n", label);
}

/*
 * jeq = Jump if equal.
 */
void jeq(int label) {
    printf("\tjeq .L%d\n", label);
}

/*
 * jne = Jump if not equal.
 */
void jne(int label) {
    printf("\tjne .L%d\n", label);
}

void cmpl(int type) {
    printf("\tcmpl (%%esp), %%eax\n");
}

/*
 * Cria um novo label.
 */
void mkLabel(int label) {
    printf(".L%d\n", label);
}

/*
 * Move valor da variável para o registrador.
 * Cópia de 32 bits.
 */
void lmove(char const *variable, int type) {
    switch(type) {
	case BYTE:
            printf("\tmovb %%al,%s\n",variable);
            break;
	case WORD:
            printf("\tmovw %%ax,%s\n",variable);
            break;
	case INTEGER:
            printf("\tmovl %%eax,%s\n",variable);
            break;
	case LONGINT:
            printf("\tmovq %%rax,%s\n",variable);
            break;
	case REAL:
            printf("\tmovss %%xmn0,%s\n",variable);
            break;
	case DOUBLE:
            printf("\tmovsd %%xmn0,%s\n",variable);
            break;
    }
}

/*
 * Move do registrador para a pilha.
 * Cópia de 32 bits.
 */
void rmove(char const *variable, int type) {
    switch(type) {
	case BYTE:
            printf("\tpushb %%al\n");
            printf("\tmovb %%al,%s\n",variable);
            break;
	case WORD:
            printf("\tpushw %%ax\n");
            printf("\tmovw %%ax,%s\n",variable);
            break;
	case INTEGER:
            printf("\tpushl %%eax\n");
            printf("\tmovl %s, %%eax\n",variable);
            break;
	case LONGINT:
            printf("\tpushq %%rax\n");
            printf("\tmovq %%rax,%s\n",variable);
            break;
	case REAL:
            printf("\tpushss %%xmn0\n");
            printf("\tmovss %%xmn0,%s\n",variable);
            break;
	case DOUBLE:
            printf("\tpushsd %%xmn0\n");
            printf("\tmovsd %%xmn0,%s\n",variable);
            break;
    }
}
