/*************************************************************************
	> File Name: intercode.h
	> Author: 
	> Mail: 
	> Created Time: Sat 21 May 2016 03:40:58 PM CST
 ************************************************************************/
#ifndef _INTERCODE_H
#define _INTERCODE_H

#include<stdio.h>
#include<string.h>
#include"semantic.h"
#include"node.h"

FILE *fp;

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef struct InterCodes_* InterCodes;

typedef enum {
    VARIABLE, CONSTANT, PARAM, LABEL, FUNCTION, TEMP, REFERENCE, ADDRESS
} OType;                        // for operands' type

typedef enum {
    LABEL_DEF, FUNC_DEF, ASSIGN, ADD, SUB, MUL, DIV, GET_ADDR, GET_REF, REFASSIGN,
    LABEL_GOTO, LABEL_REL, RETURN, DEC, ARG, CALLFUNC, I_PARAM, READ, WRITE
} IType;                         // for intercode's type

typedef enum {
    LT, GT, LE, GE, EQ, NE
} RType;                         // for relops' type 

struct Operand_ {
    OType kind;
    int isParam;
    union{
        int var_no;
        int tmp_no;
        int lab_no;
        int addr_no;
        int value;
        char *fname;              // function name
        char *pname;              // param name
    } u;
};

struct InterCode_ {
    IType kind;
    union{
        struct { Operand right, left; } assign;
        struct { Operand result, op1, op2; } binop;
        struct { Operand op; } uniop;
        struct { RType rtype; Operand label, relop1, relop2; } cond;
    } u;
};

struct InterCodes_ {
    InterCode code;
    struct InterCodes_ *prev, *next;
};

void traverse_gen_ir(char *out_fp);
Operand new_var(int no);
Operand new_tmp();
Operand new_temp(int no);
Operand new_const(int value);
Operand new_label();
Operand new_ref(int no);
Operand new_func(char *fname);
Operand new_param();
InterCodes Init_codes(InterCodes a);
InterCodes gen_assign_ir(IType itype, Operand l, Operand r);
InterCodes gen_uniop_ir(IType itype, Operand p);
InterCodes gen_binop_ir(IType itype, Operand result, Operand op1, Operand op2);
InterCodes gen_cond_ir(RType rtype, Operand label, Operand op1, Operand op2);
int cal_offset(struct SymbolMsg *s);
InterCodes codeBind(InterCodes code1, InterCodes code2);
InterCodes program(struct Node *node);
InterCodes extdeflist(struct Node *node);
InterCodes extdef(struct Node *node);
InterCodes extdeclist(struct Node *node);
InterCodes vardec(struct Node *node, Operand *place);
InterCodes fundec(struct Node *node);
InterCodes compst(struct Node *node);
InterCodes stmtlist(struct Node *node);
InterCodes stmt(struct Node *node);
InterCodes cond(struct Node *node, Operand label_true, Operand label_false);
RType get_relop(struct Node *node);
InterCodes deflist(struct Node *node);
InterCodes def(struct Node *node);
InterCodes declist(struct Node *node);
InterCodes dec(struct Node *node);
InterCodes Exp(struct Node *node, Operand place, int left);
InterCodes args(struct Node *node, Operand *arg_list, int *arg_num);
//need to be finished
InterCodes array(struct Node *node, Operand place, int left);
InterCodes structures(struct Node *node, Operand place, int left);
void print_IR(InterCodes IR, char *out_fp);
void print_op(Operand op);


#endif
