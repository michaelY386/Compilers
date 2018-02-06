/*************************************************************************
	> File Name: intercode.c
	> Author: 
	> Mail: 
	> Created Time: Mon 23 May 2016 11:02:37 AM CST
 ************************************************************************/
#include<stdio.h>
#include"stdlib.h"
#include<string.h>
#include"node.h"
#include"intercode.h"
#include<assert.h>
#include"semantic.h"

int tmp_no = 0;
int label_no = 0;

extern struct Node *root;
// access
void traverse_gen_ir(char *out_fp) {
    // traverse & gen intercode
    InterCodes IR = program(root);
    InterCodes it = IR;
    printf("after gen IR\n");
    
    while (it != NULL) {
        printf("cccc%d\n", it->code->kind);
        it = it->next;
    }

    printf("\n");
    // print intercode
    print_IR(IR, out_fp);
}

// print operands and intercodes
void print_IR(InterCodes IR, char *out_fp) {
    InterCodes ir = IR;
    fp = fopen("/home/micheal/Desktop/Compilers/Lab3/test_lab_3", "w+");
    fp = fopen(out_fp, "w+");
    while (ir != NULL) {
        switch (ir->code->kind) 
        {
            case LABEL_DEF:
                //printf("print label def\n");
                //fprintf(stderr, "NULL label!\n"); 
                //printf("%d\n", ir->code->kind);
                //if (ir->code->u.uniop.op-> == 3) printf("1248912489104761\n");
                //    else printf("110\n");
                fputs("LABEL ", fp);
                printf("########%x\n", (unsigned int)ir->code->u.uniop.op);
                print_op(ir->code->u.uniop.op);
                fputs(" :\n", fp);
                break;
            case FUNC_DEF:
                fputs("FUNCTION ", fp);
                print_op(ir->code->u.uniop.op);
                fputs(" :\n", fp);
                //printf("print func def\n");
                break;
            case ASSIGN:
                print_op(ir->code->u.assign.left);
                fputs(" := ", fp);
                print_op(ir->code->u.assign.right);
                fputs("\n", fp);
                //fprintf(stderr, "print assign\n");
                break;
            case ADD:
                print_op(ir->code->u.binop.result);
                fputs(" := ", fp);
                print_op(ir->code->u.binop.op1);
                fputs(" + ", fp);
                print_op(ir->code->u.binop.op2);
                fputs("\n", fp);
                //printf("print add\n");
                break;
            case SUB: 
                //printf("print sub\n");
                print_op(ir->code->u.binop.result);
                fputs(" := ", fp);
                print_op(ir->code->u.binop.op1);
                fputs(" - ", fp);
                print_op(ir->code->u.binop.op2);
                fputs("\n", fp);
                break;
            case MUL:   
                print_op(ir->code->u.binop.result);
                fputs(" := ", fp);
                print_op(ir->code->u.binop.op1);
                fputs(" * ", fp);
                print_op(ir->code->u.binop.op2);
                fputs("\n", fp);
                //printf("print mul\n");
                break;
            case DIV:
                print_op(ir->code->u.binop.result);
                fputs(" := ", fp);
                print_op(ir->code->u.binop.op1);
                fputs(" / ", fp);
                print_op(ir->code->u.binop.op2);
                fputs("\n", fp);
                //printf("print div\n");
                break;
            case LABEL_GOTO:
                //printf("print label_goto\n");
                fputs("GOTO ", fp);
                print_op(ir->code->u.uniop.op);
                fputs("\n", fp);
                break;
            case LABEL_REL:
                //printf("print label_true\n");
                fputs("IF ", fp);
                print_op(ir->code->u.cond.relop1);
                RType kk = ir->code->u.cond.rtype;
                if (kk == LT) fputs(" < ", fp);
                else if (kk == GT) fputs(" > ", fp);
                else if (kk == LE) fputs(" <= ", fp);
                else if (kk == GE) fputs(" >= ", fp);
                else if (kk == EQ) fputs(" == ", fp);
                else if (kk == NE) fputs(" != ", fp);
                print_op(ir->code->u.cond.relop2);
                fputs(" GOTO ", fp);
                print_op(ir->code->u.cond.label);
                fputs("\n", fp);
                //printf("print label_true\n");
                break;
            case RETURN:
                //printf("print return\n");
                fputs("RETURN ", fp);
                print_op(ir->code->u.uniop.op);
                fputs("\n", fp);
                break;
            case DEC:   // need to be finished
                //printf("print dec\n");
                fputs("DEC ", fp);
                print_op(ir->code->u.assign.left);
                fputs(" ", fp);
                fprintf(fp, "%d", ir->code->u.assign.right->u.value);
                fputs("\n", fp);
                break;
            case ARG:  // need to be finished
                //printf("print arg\n");
                fputs("ARG ", fp);
                print_op(ir->code->u.uniop.op);
                fputs("\n", fp);
                break;
            case CALLFUNC:
                //printf("print callfunc\n");
                print_op(ir->code->u.assign.left);
                fputs(" := CALL ", fp);
                print_op(ir->code->u.assign.right);
                fputs("\n", fp);
                break;
            case I_PARAM:
                fputs("PARAM ", fp);
                print_op(ir->code->u.uniop.op);
                fputs("\n", fp);
                break;
            case READ:
                //printf("print read\n");
                fputs("READ ", fp);
                print_op(ir->code->u.uniop.op);
                fputs("\n", fp);
                break;
            case WRITE:
                //printf("print write\n");
                fputs("WRITE ", fp);
                print_op(ir->code->u.uniop.op);
                fputs("\n", fp);
                break;
        }
        ir = ir->next;
        if (ir == NULL) printf("ir is null\n");
    }
    fclose(fp);
}

// to be finished
void print_op(Operand op) {
    //printf("!!!!in print_op\n");
    if (op == NULL) { printf("!!!\n"); return; }
    //printf("in print op kind %d\n", op->kind);
    //printf("12313\n");
    switch (op->kind)
    {
        case VARIABLE: //printf("v%d\n", op->u.var_no); 
                       fprintf(fp, "v%d", op->u.var_no); break;
        case CONSTANT: fprintf(fp, "#%d", op->u.value); break;
        case PARAM: fprintf(fp, "%s", op->u.pname); break;
        case LABEL: printf("in op label\n"); fprintf(fp, "label%d", op->u.lab_no); break;
        case FUNCTION: fprintf(fp, "%s", op->u.fname); break;
        case TEMP: printf("t%d\n", op->u.tmp_no); fprintf(fp, "t%d", op->u.tmp_no); break;
    }
    return;
} 


// new operands
Operand new_var(int no) {
    Operand opv = (Operand)malloc(sizeof(struct Operand_));
    //printf("in new var\n");
    opv->kind = VARIABLE;
    opv->u.var_no = no;
    printf("in new var var_no: %d\n", no);
    return opv;
}

Operand new_tmp() {
    Operand opt = (Operand)malloc(sizeof(struct Operand_));
    opt->kind = TEMP;
    opt->u.tmp_no = ++tmp_no;
    return opt;
}

Operand new_const(int value) {
    Operand opc = (Operand)malloc(sizeof(struct Operand_));
    printf("in new const %d\n", value);
    opc->kind = CONSTANT;
    opc->u.value = value;
    //printf("in new const 2\n");
    return opc;
}

Operand new_label() {
    Operand opl = (Operand)malloc(sizeof(struct Operand_));
    opl->kind = LABEL;
    //printf("in new label %d\n", opl->kind);
    opl->u.lab_no = ++label_no;
    fprintf(stderr, "in new label %d\n", opl->kind);
    return opl;
}

Operand new_ref(int no) {
    Operand opr = (Operand)malloc(sizeof(struct Operand_));
    opr->kind = REFERENCE;
    opr->u.addr_no = no;
    return opr;
}

Operand new_func(char *fname) {
    Operand opf = (Operand)malloc(sizeof(struct Operand_));
    printf("in new func op\n");
    opf->kind = FUNCTION;
    printf("in new func op 1\n");
    //printf("%s\n", fname);
    opf->u.fname = fname;                     
    fprintf(stderr, "in new func op 2\n");
    return opf;
}

Operand new_param(char *pname) {
    Operand opp = (Operand)malloc(sizeof(struct Operand_));
    opp->kind = PARAM;
    opp->u.pname = pname;                     
    return opp;
}


// generate intercodes
InterCodes Init_codes(InterCodes a) {
    a->prev = NULL;
    a->next = NULL;
    a->code = (InterCode)malloc(sizeof(struct InterCode_));
    return a;
}

InterCodes gen_assign_ir(IType itype, Operand l, Operand r) {
    InterCodes icodes = (InterCodes)malloc(sizeof(struct InterCodes_));
    icodes->prev = NULL;
    icodes->next = NULL;
    icodes->code = (InterCode)malloc(sizeof(struct InterCode_));
    //icodes = Init_codes (icodes);
    icodes->code->kind = itype;
    icodes->code->u.assign.right = r;
    icodes->code->u.assign.left = l;
    return icodes;
}

InterCodes gen_uniop_ir(IType itype, Operand p) {
    InterCodes icodes = (InterCodes)malloc(sizeof(struct InterCodes_));
    icodes->prev = NULL;
    icodes->next = NULL;
    icodes->code = (InterCode)malloc(sizeof(struct InterCode_));
    //icodes = Init_codes(icodes);
    icodes->code->kind = itype;
    //if (itype == 0) printf("000000\n");
    icodes->code->u.uniop.op = p;
    //if (icodes->code->kind == 0) printf("in gen uniop!!!! %d\n", p->u.lab_no);
    //if (icodes->code->kind == 0) fprintf(stderr, "in gen uniop!!!! %d\n", icodes->code->u.uniop.op->kind);
    printf("in gen uniop\n");
    return icodes;
}

InterCodes gen_binop_ir(IType itype, Operand result, Operand op1, Operand op2) {
    InterCodes icodes = (InterCodes)malloc(sizeof(struct InterCodes_));
    icodes->prev = NULL;
    icodes->next = NULL;
    icodes->code = (InterCode)malloc(sizeof(struct InterCode_));
    icodes->code->kind = itype;
    icodes->code->u.binop.op1 = op1;
    icodes->code->u.binop.op2 = op2;
    icodes->code->u.binop.result = result;
    return icodes;
}

InterCodes gen_cond_ir(RType rtype, Operand label, Operand op1, Operand op2) {
    InterCodes icodes = (InterCodes)malloc(sizeof(struct InterCodes_));
    icodes->prev = NULL;
    icodes->next = NULL;
    icodes->code = (InterCode)malloc(sizeof(struct InterCode_));
    icodes->code->kind = LABEL_REL;
    icodes->code->u.cond.rtype = rtype;
    icodes->code->u.cond.label = label;
    icodes->code->u.cond.relop1 = op1;
    icodes->code->u.cond.relop2 = op2;
    return icodes;
}

// traverse syntax tree
InterCodes program(struct Node *node) {          //??
    //printf("in program\n"); 
    struct Node *child = node->arity;
    return extdeflist(child);
}

InterCodes extdeflist(struct Node *node) {            
    //printf("in extdeflist\n"); 
    struct Node *child = node->arity;
    InterCodes icodes1, icodes2;
    InterCodes icodes;
    icodes1 = extdef(child);
    child = child->nextsibling;
    //if (child->arity == NULL) icodes2 = NULL;
    if (child != NULL )icodes2 = extdeflist(child);
    else icodes2 = NULL;
    icodes = codeBind(icodes1, icodes2);
    return icodes;
}

InterCodes extdef(struct Node *node) {
    struct Node *child = node->arity;
    assert(child != NULL);
    //printf("in extdef\n"); 
    if (strcmp(child->nextsibling->type, "ExtDecList") == 0) {
        //printf("in extdef if\n"); 
        return extdeclist(child->nextsibling);
    }
    else if (strcmp(child->nextsibling->type, "FunDec") == 0) {
        InterCodes icodes, icodes1, icodes2;
        //printf("in extdef if\n"); 
        icodes1 = fundec(child->nextsibling);
        icodes2 = compst(child->nextsibling->nextsibling);
        icodes = codeBind(icodes1, icodes2);
        return icodes;
    }
    return NULL;
}

InterCodes extdeclist(struct Node *node) {
    struct Node *child = node->arity;
    //printf("in extdeclist\n"); 
    assert(child != NULL);
    if (child->nextsibling != NULL) {
        return vardec(child, NULL);
    }
    else {
        InterCodes icodes, icodes1, icodes2;
        icodes1 = vardec(child, NULL);
        icodes2 = extdeclist(child->nextsibling->nextsibling);
        icodes = codeBind(icodes1, icodes2);
        return icodes;
    }
    return NULL;
}

InterCodes vardec(struct Node *node, Operand *place) {             //??
    struct Node *child = node->arity;
    //printf("in vardec\n"); 
    assert(child != NULL);
    if (strcmp(child->type, "ID") == 0) {
        // if search and it is float or int then return nothing
        enum KType x;
        int vari_no = lookup_stable(child->text, &x);
        Operand v = new_var(vari_no);
        //printf("in vardec %d\n", vari_no);
        if (place != NULL) *place = v;
        if (x == INT || x == FLOAT) return NULL;
    }
    else {
        return vardec(child, NULL);
    }
}

InterCodes fundec(struct Node *node) {
    struct Node *child = node->arity;
    assert(child != NULL);
    //printf("in fundec\n");
    InterCodes codes;
    //printf("in fundec 1\n");
    struct FuncMsg *fmsg = lookup_ftable(child->text);
    //printf("in fundec 2\n");
    Operand f = new_func(child->text);
    //printf("in fundec 3\n");
    InterCodes fcodes = gen_uniop_ir(FUNC_DEF, f);
    if (strcmp(child->nextsibling->nextsibling->type, "VarList") == 0) {
        struct ArgList *par = fmsg->Args;
        while (par != NULL) {
            if (par->argType == 2)
                printf("Cannnot Translate: Code contains variables of multidimention array type of parameters of array type.\n");
            else {
                enum KType x;
                int a = lookup_stable(par->name, &x); 
                Operand p = new_var(a);
                InterCodes param_codes = gen_uniop_ir(I_PARAM, p);
                codes = fcodes;
                codes = codeBind(codes, param_codes); 
            }
            par = par->nextarg;
        }
        return codes;
    }
    return fcodes;                               //ID LP RP    
}

InterCodes compst(struct Node *node) {
    struct Node *child = node->arity;
    InterCodes icodes, icodes1, icodes2;
    icodes1 = NULL;
    icodes2 = NULL;
    assert(child != NULL);
    //printf("in compst\n");
    child = child->nextsibling;
    if (child != NULL && strcmp(child->type, "DefList") == 0) {
        icodes1 = deflist(child);
        child = child->nextsibling;
    }
    if (child != NULL && strcmp(child->type, "StmtList") == 0) {
        icodes2 = stmtlist(child);
    }
    icodes = codeBind(icodes1, icodes2);
    return icodes;
}

InterCodes stmtlist(struct Node *node) {
    if (node == NULL) return NULL;
    //printf("in stmtlist\n");
    struct Node *child = node->arity;
    assert(child != NULL);
    InterCodes icodes, icodes1, icodes2;
    icodes1 = stmt(child);
    icodes2 = stmtlist(child->nextsibling);
    icodes = codeBind(icodes1, icodes2);
    return icodes;
}

InterCodes stmt(struct Node *node) {
    struct Node *child = node->arity;
    assert(child != NULL);
    //printf("in stmt\n");
    Operand t, label1, label2, label3;
    InterCodes icodes1, icodes2, icodes3, icodes4, icodes5, icodes6, icodes7;
    InterCodes icodes;
    if (strcmp(child->type, "Exp") == 0) {
        return Exp(child, NULL);
    }
    else if (strcmp(child->type, "CompSt") == 0) {
        return compst(child);
    }
    else if (strcmp(child->type, "RETURN") == 0) {
        t = new_tmp();
        icodes1 = Exp(child->nextsibling, t);
        icodes2 = gen_uniop_ir(RETURN, t);
        icodes = codeBind(icodes1, icodes2);
        //printf("in stmt return\n");
        return icodes;
    }
    else if (strcmp(child->type, "IF") == 0) {
        if (child->nextsibling->nextsibling->nextsibling->nextsibling->nextsibling == NULL) {
            //printf("in stmt if 1\n");
            label1 = new_label();
            label2 = new_label();
            icodes1 = cond(child->nextsibling->nextsibling, label1, label2);
            //fprintf(stderr, "in stmt if %d %d\n", label1->kind, label2->kind);
            icodes2 = stmt(child->nextsibling->nextsibling->nextsibling->nextsibling);
            //fprintf(stderr, "fuck\n");
            icodes3 = gen_uniop_ir(LABEL_DEF, label1);
            icodes4 = gen_uniop_ir(LABEL_DEF, label2);
            //printf("in stmt if 3\n");
            icodes = codeBind(icodes1, icodes3);
            icodes = codeBind(icodes, icodes2);
            icodes = codeBind(icodes, icodes4);
            //printf("in stmt if 4\n");
            //fprintf(stderr, "wtf %d\n", icodes2->prev->code->u.uniop.op->kind);
            //printf("#########%x\n", (unsigned int)icodes3->code->u.uniop.op);
            //fprintf(stderr, "wtf %d\n", icodes4->code->u.uniop.op->kind);
            return icodes;
        }
        else {
            //printf("in stmt if else\n");
            label1 = new_label();
            label2 = new_label();
            label3 = new_label();
            icodes1 = cond(child->nextsibling->nextsibling, label1, label2);
            icodes2 = stmt(child->nextsibling->nextsibling->nextsibling->nextsibling);
            fprintf(stderr, "fuck!\n");
            icodes3 = stmt(child->nextsibling->nextsibling->nextsibling->nextsibling->nextsibling->nextsibling);
            icodes4 = gen_uniop_ir(LABEL_DEF, label1);
            icodes5 = gen_uniop_ir(LABEL_DEF, label2);
            icodes6 = gen_uniop_ir(LABEL_DEF, label3);
            icodes7 = gen_uniop_ir(LABEL_GOTO, label3);
            icodes = codeBind(codeBind(codeBind(codeBind(codeBind(codeBind(icodes1, icodes4), icodes2), icodes7), icodes5), icodes3), icodes6);
            return icodes;
        }
    }
    else if (strcmp(child->type, "WHILE") == 0) {
            label1 = new_label();
            label2 = new_label();
            label3 = new_label();
            icodes1 = cond(child->nextsibling->nextsibling, label2, label3);
            icodes2 = stmt(child->nextsibling->nextsibling->nextsibling->nextsibling);
            icodes3 = gen_uniop_ir(LABEL_DEF, label1);
            icodes4 = gen_uniop_ir(LABEL_DEF, label2);
            icodes5 = gen_uniop_ir(LABEL_DEF, label3);
            icodes6 = gen_uniop_ir(LABEL_GOTO, label1);
            icodes = codeBind(codeBind(codeBind(codeBind(codeBind(icodes3, icodes1), icodes4), icodes2), icodes6), icodes5);
            return icodes;
    }
}

InterCodes cond(struct Node *node, Operand label_true, Operand label_false) {
    struct Node *child = node->arity;
    assert(child != NULL);
    Operand t1, t2, v, label, t, c;
    InterCodes icodes;
    InterCodes icodes1, icodes2, icodes3, icodes4;
    RType op;
    if (strcmp(child->type, "NOT") == 0) {
        return cond(child->nextsibling, label_false, label_true);
    }
    if (strcmp(child->type, "Exp") == 0) {
        if (child->nextsibling != NULL && strcmp(child->nextsibling->type, "RELOP") == 0) {
            printf("in cond exp null\n");
            t1 = new_tmp();
            t2 = new_tmp();
            icodes1 = Exp(child, t1);
            icodes2 = Exp(child->nextsibling->nextsibling, t2);
            op = get_relop(child->nextsibling);
            //printf("op : %d\n", op);
            icodes3 = gen_cond_ir(op, label_true, t1, t2);
            icodes = codeBind(codeBind(icodes1, icodes2), icodes3);
            icodes4 = gen_uniop_ir(LABEL_GOTO, label_false);
            icodes = codeBind(icodes, icodes4);
            //printf("in cond\n");
            return icodes;
        }
        else if (child->nextsibling != NULL && strcmp(child->nextsibling->type, "AND") == 0) {
            printf("in cond exp and\n");
            label = new_label();
            icodes1 = cond(child, label, label_false);
            icodes2 = cond(child->nextsibling->nextsibling, label_true, label_false);
            icodes3 = gen_uniop_ir(LABEL_DEF, label);
            icodes = codeBind(codeBind(icodes1, icodes3), icodes2);
            return icodes;
        }
        else if (child->nextsibling != NULL && strcmp(child->nextsibling->type, "OR") == 0) {
            printf("in cond exp or\n");
            label = new_label();
            icodes1 = cond(child, label_true, label);
            icodes2 = cond(child->nextsibling->nextsibling, label_true, label_false);
                icodes3 = gen_uniop_ir(LABEL_DEF, label);
                icodes = codeBind(codeBind(icodes1, icodes3), icodes2);
                return icodes;
            }
        }
        else {
            t = new_tmp();
            icodes1 = Exp(node, t);
            c = new_const(0);
            icodes2 = gen_cond_ir(NE, label_true, t, c);
            icodes = codeBind(icodes1, icodes2);
            icodes3 = gen_uniop_ir(LABEL_GOTO, label_false);
            icodes = codeBind(icodes, icodes3);
            return icodes;
        }
    }

RType get_relop(struct Node *node) {
        if (strcmp(node->text, "<") == 0) return LT;
        else if (strcmp(node->text, ">") == 0) return GT;
        else if (strcmp(node->text, "<=") == 0) return LE;
        else if (strcmp(node->text, ">=") == 0) return GE;
        else if (strcmp(node->text, "==") == 0) return EQ;
        else if (strcmp(node->text, "!=") == 0) return NE;
        else { 
            // need to be finished
        }
}

InterCodes deflist(struct Node *node) {
        if (node == NULL) return NULL;
        struct Node *child = node->arity;
        assert(child != NULL);
        InterCodes icodes, icodes1, icodes2;
        icodes1 = def(child);
        icodes2 = deflist(child->nextsibling);
        icodes = codeBind(icodes1, icodes2);
        return icodes;
    }

InterCodes def(struct Node *node) {
        struct Node *child = node->arity;
        assert(child != NULL);
        child = child->nextsibling;
        return declist(child);
    }

InterCodes declist(struct Node *node) {
        struct Node *child = node->arity;
        assert(child != NULL);
        if (child->nextsibling == NULL) {
            return dec(child);
        }
        else {
            InterCodes icodes, icodes1, icodes2;
            icodes1 = dec(child);
            icodes2 = declist(child->nextsibling->nextsibling);
            icodes = codeBind(icodes1, icodes2);
            return icodes;
        }
    }

InterCodes dec(struct Node *node) {
        struct Node *child = node->arity;
        assert(child != NULL);
        if (child->nextsibling == NULL) {
            return vardec(child, NULL);
        }
        else {
            InterCodes icodes, icodes1, icodes2;
            Operand t1, t2;
            icodes1 = vardec(child, &t1);
            t2 = new_tmp();
            printf("in dec\n");
            icodes2 = Exp(child->nextsibling->nextsibling, t2);
            //if (t1 == NULL) printf("!!!!!!!!@#@#@@#@@\n");
            //else printf("%d\n", tmp_no);
            icodes = codeBind(icodes1, icodes2);
            InterCodes icodes3;
            icodes3 = gen_assign_ir(ASSIGN, t1, t2);
            icodes = codeBind(icodes, icodes3);
            return icodes;
        }
    }

InterCodes Exp(struct Node *node, Operand place) {
        assert(node != NULL);
        struct Node *child = node->arity;
        assert(child != NULL);
        InterCodes icodes1, icodes2, icodes3;
        InterCodes icodes;
        Operand c, c1, c2, v, t, t1, t2, label1, label2, f;
        int a, i;
        enum KType x;
        //printf("&&&&& %s\n", child->type);
        if (strcmp(child->type, "INT") == 0 || strcmp(child->type, "FLOAT") == 0) {
            //printf("in exp int\n");
            //printf("%s\n", child->text);
            Operand c = new_const(atoi(child->text));
            if (place != NULL) icodes = gen_assign_ir(ASSIGN, place, c);
            //printf("in exp int :%d\n", c->u.value);
            return icodes;
        }
        else if (strcmp(child->type, "ID") == 0 && child->nextsibling == NULL) {
            //printf("in exp id\n");
            a = lookup_stable(child->text, &x);
            v = new_var(a);
            //printf("in exp id 2\n");
            if (place != NULL) icodes = gen_assign_ir(ASSIGN, place, v);
            return icodes;
        }
        //need to be finished 
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "ASSIGNOP") == 0) {
            //printf("!!!!!\n");
            //printf("%s\n", child->arity->text);
            a = lookup_stable(child->arity->text, &x);
            //printf("a: %d\n", a);
            v = new_var(a);
            t = new_tmp();
            icodes1 = Exp(child->nextsibling->nextsibling, t);
            icodes2 = gen_assign_ir(ASSIGN, v, t);
            icodes = codeBind(icodes1, icodes2);
            // need to be finished array
            if (place != NULL) {
                icodes3 = gen_assign_ir(ASSIGN, place, v);
                icodes = codeBind(icodes, icodes3);
            }
            return icodes;
        }                                           
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "PLUS") == 0) {
            t1 = new_tmp();
            t2 = new_tmp();
            icodes1 = Exp(child, t1);
            icodes2 = Exp(child->nextsibling->nextsibling, t2);
            if (place != NULL) icodes3 = gen_binop_ir(ADD, place, t1, t2);
            icodes = codeBind(icodes1, icodes2);
            icodes = codeBind(icodes, icodes3);
            return icodes;
        }
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "MINUS") == 0) {
            t1 = new_tmp();
            t2 = new_tmp();
            icodes1 = Exp(child, t1);
            icodes2 = Exp(child->nextsibling->nextsibling, t2);
            if (place != NULL) icodes3 = gen_binop_ir(SUB, place, t1, t2);
            icodes = codeBind(icodes1, icodes2);
            icodes = codeBind(icodes, icodes3);
            return icodes;
        }
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "STAR") == 0) {
            printf("in exp star exp\n");
            t1 = new_tmp();
            t2 = new_tmp();
            icodes1 = Exp(child, t1);
            icodes2 = Exp(child->nextsibling->nextsibling, t2);
            if (place != NULL) icodes3 = gen_binop_ir(MUL, place, t1, t2);
            icodes = codeBind(icodes1, icodes2);
            icodes = codeBind(icodes, icodes3);
            return icodes;
        }
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "DIV") == 0) {
            t1 = new_tmp();
            t2 = new_tmp();
            icodes1 = Exp(child, t1);
            icodes2 = Exp(child->nextsibling->nextsibling, t2);
            if (place != NULL) icodes3 = gen_binop_ir(DIV, place, t1, t2);
            icodes = codeBind(icodes1, icodes2);
            icodes = codeBind(icodes, icodes3);
            return icodes;
        }
        else if (strcmp(child->type, "MINUS") == 0) {
            t = new_tmp();
            c = new_const(0);
            icodes1 = Exp(child->nextsibling, t);
            icodes2 = gen_binop_ir(SUB, place, c, t);
            icodes = codeBind(icodes1, icodes2);
            return icodes;
        }
    else if (strcmp(child->type, "NOT") == 0 || strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "RELOP") == 0 || strcmp(child->nextsibling->type, "AND") == 0 || strcmp(child->nextsibling->type, "OR") == 0) {
        //printf("virus1\n");
        label1 = new_label();
        label2 = new_label();
        c1 = new_const(0);
        c2 = new_const(1);
        InterCodes icodes0 = gen_assign_ir(ASSIGN, place, c1);
        icodes1 = cond(node, label1, label2);
        icodes2 = gen_uniop_ir(LABEL_DEF, label1);
        icodes3 = gen_assign_ir(ASSIGN, place, c2);
        InterCodes icodes4 = gen_uniop_ir(LABEL_DEF, label2);
        icodes = codeBind(codeBind(codeBind(codeBind(icodes0, icodes1), icodes2), icodes3),icodes4);
        return icodes;
    }
    else if (strcmp(child->type, "ID") == 0 && strcmp(child->nextsibling->type, "LP") == 0) {
        if (strcmp(child->nextsibling->nextsibling->type, "RP") == 0) {
            if (strcmp(child->text, "read") == 0) {
                icodes = gen_uniop_ir(READ, place);
                return icodes;
            }
            else {
                struct FuncMsg *fmsg = lookup_ftable(child->text);
                f = new_func(child->text);
                if (place != NULL) {
                    //need to be finished
                    icodes = gen_assign_ir(CALLFUNC, place, f);
                }
                else {
                    t1 = new_tmp();
                    icodes = gen_assign_ir(CALLFUNC, t1, f);
                }
                return icodes;
            }
        }
        else if (strcmp(child->nextsibling->nextsibling->type, "Args") == 0) {
            //need to be finished
            int arg_num = 0;
            Operand *arg_list = (Operand *)malloc(sizeof(Operand) * 10);   //????
            struct FuncMsg *fmsg = lookup_ftable(child->text);
            f = new_func(child->text);
            icodes1 = args(child->nextsibling->nextsibling, arg_list, &arg_num);
            if (strcmp(child->text, "write") == 0) {
                //printf("arg_num: %d\n", arg_num);
                if (arg_num != 1) printf("write args wrong!\n");
                t = arg_list[0];
                icodes2 = gen_uniop_ir(WRITE, t);
                icodes = codeBind(icodes1, icodes2);
                return icodes;
            }
            else 
                for (i = 0; i < arg_num; i++) {
                    icodes2 = gen_uniop_ir(ARG, arg_list[i]);
                    icodes1 = codeBind(icodes1, icodes2);
                }
            if (place != NULL) icodes3 = gen_assign_ir(CALLFUNC, place, f);
            else {
                t = new_tmp();
                icodes3 = gen_assign_ir(CALLFUNC, t, f);
            } 
            icodes = codeBind(icodes1, icodes3);
            return icodes;
        }
    }
    else if (strcmp(child->type, "LP" ) == 0) {
        return Exp(child->nextsibling, place);
    }
}

//need to be finished
InterCodes args(struct Node *node, Operand *arg_list, int *arg_num) {
    struct Node *child = node->arity;
    InterCodes icodes1, icodes2;
    InterCodes icodes;
    enum KType x;
    assert(child != NULL);
    //fprintf(stderr, "in args\n");
    Operand t;
    if (child->nextsibling == NULL) {
        //int a = lookup_stable(child->arity->text, &x);  
        t = new_tmp();
        icodes = Exp(child, t);
        //printf("in args@@ %d\n", icodes->code->u.uniop.op->kind);
        //printf("in args 1 %d\n", *arg_num);
        arg_list[*arg_num] = t;
        (*arg_num)++;
        //printf("in args@@ %d\n", *arg_num);
        return icodes;
    }
    else if (strcmp(child->nextsibling->type, "COMMA") == 0) {
        t = new_tmp();
        icodes1 = Exp(child, t);
        arg_list[*(arg_num)++] = t;
        icodes2 = args(child->nextsibling->nextsibling, arg_list, arg_num);    //????? &
        icodes = codeBind(icodes1, icodes2);
        return icodes;
    }
}
/*
//need to be finished
InterCodes structure(struct Node *node, Operand place) {
    struct Node *child = node->arity;
    enum KType x;
    int offset;
    assert(child != NULL);

    if (strcmp(child->arity->type, "ID") == 0) {
        int a = lookup_stable(child->arity->text, &x);
        char *fiename;
        struct SymbolMsg *sMsg = getVar(child->arity->text);
        struct FieldMsg *fie = sMsg->fieldList;
        if (fie == NULL) printf("!!!!!\n");
        strcmp(fiename, child->nextsibling->nextsibling->text);
        offset = 0;
        while (fie != NULL) {
            if (strcmp(fie->name, fiename) == 0) break;
            struct SymbolMsg *ss = getVar(fie->name);
            offset = offset + cal_offset(ss);
            fie = fie->nextField;
        }
    }
}

//need to be finished
InterCodes array() {
    
}

//???????  need to be finished
int cal_offset(struct SymbolMsg *s) {
    if (s->isArray == 0 && s->symType == INT || s->symType == FLOAT) {
        return 4;
    }
    else if (s->symType == STRUCT) {
        int off = 0;
        struct FieldMsg *flist = s->fieldList;
        while (flist != NULL) {
            struct SymbolMsg *sis = getVar(flist->name);
            off = off + cal_offset(sis);
            flist = flist->nextField;
        }
        return off;
    }
    else if (s->isArray == 1) {
        int off = 4;
        struct ArrMsg *aMsg = s->arr;
        while (aMsg != NULL) {
            off = off * aMsg->size;
            aMsg = aMsg->nextarrType;
        }
        return off;
    }
    return 0;
}
*/
// binding of intercodes
InterCodes codeBind(InterCodes code1, InterCodes code2) {
    if (code1 == NULL) return code2;
    if (code2 == NULL) return code1;
    InterCodes p = code1;
    while (p->next != NULL) {
        //printf("%d\n", p->code->kind);
        p = p->next;
    }
    //printf("!!!\n");
    p->next = code2;
    code2->prev = p;
    return code1;
}
