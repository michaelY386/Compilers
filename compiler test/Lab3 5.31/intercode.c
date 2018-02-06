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
    /* 
    while (it != NULL) {
        printf("cccc%d\n", it->code->kind);
        it = it->next;
    }
    */
    printf("\n");
    // print intercode
    print_IR(IR, out_fp);
}


// new operands
Operand new_var(int no) {
    Operand opv = (Operand)malloc(sizeof(struct Operand_));
    //printf("in new var\n");
    opv->kind = VARIABLE;
    opv->u.var_no = no;
    //printf("in new var var_no: %d\n", no);
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
    //printf("in new const %d\n", value);
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
    //fprintf(stderr, "in new label %d\n", opl->kind);
    return opl;
}

Operand new_ref(int no) {
    Operand opr = (Operand)malloc(sizeof(struct Operand_));
    opr->kind = REFERENCE;
    opr->u.addr_no = no;
    //printf("in new ref%d\n", no);
    return opr;
}

Operand new_func(char *fname) {
    Operand opf = (Operand)malloc(sizeof(struct Operand_));
    opf->kind = FUNCTION;
    opf->u.fname = fname;                     
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
    icodes->code->u.uniop.op = p;
    //printf("in gen uniop\n");
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

//for array and structures
int cal_offset(struct SymbolMsg *s) {
    //printf("in call offset\n");
    if (s->isArray == 0 && s->symType == INT || s->symType == FLOAT) {
        return 4;
    }
    else if (s->symType == STRUCT) {
        //printf("in calloff struct\n");
        int time = 0, off = 0;
        struct FieldMsg *flist = s->fieldList;
        while (flist != NULL) {
            time = time + 1;
            struct SymbolMsg *sis = getVar(flist->name);
            off = off + cal_offset(sis);
            flist = flist->nextField;
        }
        //printf("time: %d\n", time);
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
        //printf("in vardec %s\n", child->text);
        struct SymbolMsg *msg = getVar(child->text);
        //if (msg->isArray == 0) printf("in vardec it is not an array\n");
        Operand v = new_var(vari_no);
        //printf("in vardec %d\n", vari_no);
        if (place != NULL) *place = v;
        if (x == INT && msg->isArray == 0) return NULL;
        else if (msg->isArray == 1) {
            //printf("in vardec array\n");
            int r = msg->arr->size;
            r = r * 4;
            Operand c = new_const(r);
            InterCodes icodes = gen_assign_ir(DEC, v, c);
        }
        else if (msg->symType == STRUCT) {
            //printf("in vardec struct\n");
            msg = getVar(msg->strtype);
            //printf("%s\n", msg->strtype);
            int r = cal_offset(msg);
            Operand c = new_const(r);
            InterCodes icodes = gen_assign_ir(DEC, v, c);
        }
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
    InterCodes fcodes = gen_uniop_ir(FUNC_DEF, f);
    if (strcmp(child->nextsibling->nextsibling->type, "VarList") == 0) {
        struct ArgList *par = fmsg->Args;
        while (par != NULL) {
            if (par->argType == 2)
                printf("Cannnot Translate: Code contains variables of multidimention array type of parameters of array type.\n");
            else {
                enum KType x;
                int a = lookup_stable(par->name, &x); 
                //printf("in fundec %d\n", a);
                Operand p = new_var(a);
                InterCodes param_codes = gen_uniop_ir(I_PARAM, p);
                struct SymbolMsg *sss = getVar_str(par->name);
                struct SymbolMsg *ss = getVar(par->name);
                //printf("!!!!!2@@ %d\n", ss->isParam);
                //p->isParam = 1;
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
        return Exp(child, NULL, 0);
    }
    else if (strcmp(child->type, "CompSt") == 0) {
        return compst(child);
    }
    else if (strcmp(child->type, "RETURN") == 0) {
        t = new_tmp();
        //printf("in stmt return\n");
        icodes1 = Exp(child->nextsibling, t, 1);
        icodes2 = gen_uniop_ir(RETURN, t);
        icodes = codeBind(icodes1, icodes2);
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
            //printf("in cond exp null\n");
            t1 = new_tmp();
            t2 = new_tmp();
            icodes1 = Exp(child, t1, 0);
            icodes2 = Exp(child->nextsibling->nextsibling, t2, 0);
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
            //printf("in cond exp and\n");
            label = new_label();
            icodes1 = cond(child, label, label_false);
            icodes2 = cond(child->nextsibling->nextsibling, label_true, label_false);
            icodes3 = gen_uniop_ir(LABEL_DEF, label);
            icodes = codeBind(codeBind(icodes1, icodes3), icodes2);
            return icodes;
        }
        else if (child->nextsibling != NULL && strcmp(child->nextsibling->type, "OR") == 0) {
            //printf("in cond exp or\n");
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
            icodes1 = Exp(node, t, 0);   //??????????left or right?
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
            icodes2 = Exp(child->nextsibling->nextsibling, t2, 0);
            //if (t1 == NULL) printf("!!!!!!!!@#@#@@#@@\n");
            //else printf("%d\n", tmp_no);
            icodes = codeBind(icodes1, icodes2);
            InterCodes icodes3;
            icodes3 = gen_assign_ir(ASSIGN, t1, t2);
            icodes = codeBind(icodes, icodes3);
            return icodes;
        }
    }

InterCodes Exp(struct Node *node, Operand place, int left) {
        assert(node != NULL);
        struct Node *child = node->arity;
        assert(child != NULL);
        //printf("****in exp %d \n", left);
        InterCodes icodes1 = NULL, icodes2 = NULL, icodes3 = NULL, icodes4 = NULL;
        InterCodes icodes;
        Operand c, c1, c2, v, t, t1, t2, label1, label2, f;
        int a, i;
        enum KType x;
        //printf("&&&&& %s\n", child->type);
        //printf("%s %s\n", child->type, child->nextsibling->type);
        if (strcmp(child->type, "INT") == 0) {
            //printf("in exp int\n");
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
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "LB") == 0) {
            //printf("inarray?????\n");
            InterCodes acodes = array(child, place, left);
            return acodes;
        }
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "DOT") == 0) {
            //printf("in exp struct\n");
            InterCodes scodes = structures(child, place, left);
            return scodes;
        }
        //need to be finished 
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "ASSIGNOP") == 0) {
            //printf("!!!!!\n");
            //printf("%s\n", child->arity->text);
            if (strcmp(child->arity->type, "ID") == 0) {
                a = lookup_stable(child->arity->text, &x);
                //printf("a: %d\n", a);
                v = new_var(a);
                t = new_tmp();
                icodes1 = Exp(child->nextsibling->nextsibling, t, 0);
                icodes2 = gen_assign_ir(ASSIGN, v, t);
                icodes = codeBind(icodes1, icodes2);
                // need to be finished array
                if (place != NULL) {
                    icodes3 = gen_assign_ir(ASSIGN, place, v);
                    icodes = codeBind(icodes, icodes3);
                }
                return icodes;
            }
            t1 = new_tmp();
            t2 = new_tmp();
            //printf("in exp -> exp = exp array\n");
            icodes1 = Exp(child, t1, 1);
            icodes2 = Exp(child->nextsibling->nextsibling, t2, 0);
            Operand t3 = new_tmp();
            t3->kind = TEMP;
            //printf("to here 100\n");
            t3->u.tmp_no = t1->u.tmp_no;
            struct Node *temp = child;
            while (strcmp(temp->type, "ID") != 0 && temp != NULL) temp = temp->arity;
            struct SymbolMsg *smsg = getVar(temp->text);
            if (smsg->isArray == 1 || smsg->symType == STRUCT) t3->kind = ADDRESS;
            //icodes5 = gen_assign_ir(ASSIGN, place, t4);
            //icodes = codeBind(icodes, icodes5);
            //icodes3 = NULL;
            //printf("wtf %d\n", t1->kind);
            
            icodes3 = gen_assign_ir(ASSIGN, t3, t2);
            icodes = codeBind(codeBind(icodes1, icodes2), icodes3);
            icodes4 = NULL;
            if (place != NULL){
                icodes4 = gen_assign_ir(ASSIGN, place, t1); 
            }
            icodes = codeBind(icodes, icodes4);
            //printf("to here 110\n");
            return icodes;
        }
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "PLUS") == 0) {
            t1 = new_tmp();
            t2 = new_tmp();
            icodes1 = Exp(child, t1, 0);
            icodes2 = Exp(child->nextsibling->nextsibling, t2, 0);
            icodes3 = NULL;
            if (place != NULL) icodes3 = gen_binop_ir(ADD, place, t1, t2);
            icodes = codeBind(icodes1, icodes2);
            icodes = codeBind(icodes, icodes3);
            return icodes;
        }
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "MINUS") == 0) {
            t1 = new_tmp();
            t2 = new_tmp();
            icodes1 = Exp(child, t1, 0);
            icodes2 = Exp(child->nextsibling->nextsibling, t2, 0);
            icodes3 = NULL;
            if (place != NULL) icodes3 = gen_binop_ir(SUB, place, t1, t2);
            icodes = codeBind(icodes1, icodes2);
            icodes = codeBind(icodes, icodes3);
            return icodes;
        }
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "STAR") == 0) {
            //printf("in exp star exp\n");
            t1 = new_tmp();
            t2 = new_tmp();
            icodes1 = Exp(child, t1, 0);
            icodes2 = Exp(child->nextsibling->nextsibling, t2, 0);
            icodes3 = NULL;
            if (place != NULL) icodes3 = gen_binop_ir(MUL, place, t1, t2);
            icodes = codeBind(icodes1, icodes2);
            icodes = codeBind(icodes, icodes3);
            return icodes;
        }
        else if (strcmp(child->type, "Exp") == 0 && strcmp(child->nextsibling->type, "DIV") == 0) {
            t1 = new_tmp();
            t2 = new_tmp();
            icodes1 = Exp(child, t1, 0);
            icodes2 = Exp(child->nextsibling->nextsibling, t2, 0);
            icodes3 = NULL;
            if (place != NULL) icodes3 = gen_binop_ir(DIV, place, t1, t2);
            icodes = codeBind(icodes1, icodes2);
            icodes = codeBind(icodes, icodes3);
            return icodes;
        }
        else if (strcmp(child->type, "MINUS") == 0) {
            t = new_tmp();
            c = new_const(0);
            icodes1 = Exp(child->nextsibling, t, 0);
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
            //printf("in exp(args)\n");
            int arg_num = 0;
            Operand *arg_list = (Operand *)malloc(sizeof(Operand) * 10);   //????
            struct FuncMsg *fmsg = lookup_ftable(child->text);
            f = new_func(child->text);
            //printf("in exp(args)\n");
            icodes1 = args(child->nextsibling->nextsibling, arg_list, &arg_num);
            //printf("in exp(args)\n");
            if (strcmp(child->text, "write") == 0) {
                if (arg_num != 1) printf("write args wrong!\n");
                t = arg_list[0];
                icodes2 = gen_uniop_ir(WRITE, t);
                icodes = codeBind(icodes1, icodes2);
                return icodes;
            }
            else 
                {
                //printf("arg_num: %d\n", arg_num);
                // the args are inversed
                for (i = arg_num-1; i >= 0; i--) {
                    icodes2 = gen_uniop_ir(ARG, arg_list[i]);
                    icodes1 = codeBind(icodes1, icodes2);
                    }
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
            return Exp(child->nextsibling, place, 0);
    }
}

InterCodes args(struct Node *node, Operand *arg_list, int *arg_num) {
    struct Node *child = node->arity;
    InterCodes icodes1, icodes2;
    InterCodes icodes;
    enum KType x;
    assert(child != NULL);
    //fprintf(stderr, "in args\n");
    Operand t, t1, t2;
    if (child->nextsibling == NULL) {
        if (strcmp(child->arity->type, "ID") == 0) {
            struct SymbolMsg *s = getVar(child->arity->text);
            int a = lookup_stable(child->arity->text, &x);
            if (s->symType == STRUCT || s->isArray == 1) {
                    t1 = new_ref(a);
                    //printf("args no!!!!!!:  %d\n", a);
                    //t1->kind = VARIABLE;
                    t2 = new_tmp();
                    icodes1 = gen_assign_ir(ASSIGN, t2, t1);
                    arg_list[*arg_num] = t2;
                    *arg_num = *arg_num + 1;
                    return icodes1;
                }
            }
        t = new_tmp();
        icodes = Exp(child, t, 0);
        //printf("arg_num: %d\n",  *arg_num);
        //arg_list[(*arg_num)++] = t;
        arg_list[*arg_num] = t;
        *arg_num = *arg_num + 1;
        return icodes;
    }
    else if (strcmp(child->nextsibling->type, "COMMA") == 0) {
        t = new_tmp();
        icodes1 = Exp(child, t, 0);
        //printf("in args comma\n");
        arg_list[*arg_num] = t;
        *arg_num = *arg_num + 1;
        //printf("arg_num!: %d\n",  *arg_num);
        icodes2 = args(child->nextsibling->nextsibling, arg_list, arg_num);    //????? &
        icodes = codeBind(icodes1, icodes2);
        return icodes;
    }
}

InterCodes array(struct Node *node, Operand place, int left) {
    //struct Node *child = node->child;
    //assert(child != NULL);
    //child = child->nextsibling;
    printf("in array!!!!\n");
    InterCodes icodes1, icodes2, icodes3, icodes4, icodes5, icodes;
    Operand c, t1, t2, t3, t4, t5, t6;
    int a, offset;
    enum KType x;
    if (node->arity->nextsibling != NULL && strcmp(node->arity->nextsibling->type, "LB") == 0) {
        printf("Cannot translate: Code contains varibles of multi-dimension array type or parameters of array type.\n");
        exit(0);
    }
    offset = 4;
    c = new_const(offset);
    if (strcmp(node->arity->type,"ID") == 0) {
        a = lookup_stable(node->arity->text, &x);
        t1 = new_ref(a);
        printf("array no!!!!!!:  %d\n", a);
    }
    t6 = new_tmp();
    t5 = new_tmp();
    t2 = new_tmp();
    t3 = new_tmp();
    t4 = new_tmp();
    t6 = new_tmp();
    icodes4 = gen_assign_ir(ASSIGN, t5, t1); 
    icodes1 = Exp(node->nextsibling->nextsibling, t2, 0);
    icodes2 = gen_binop_ir(MUL, t3, t2, c);
    icodes3 = gen_binop_ir(ADD, t4, t3, t5);
    icodes = codeBind(codeBind(codeBind(icodes4, icodes1), icodes2), icodes3);
    if (left == 0) 
        {
            t6->u.tmp_no = t4->u.tmp_no;
            t6->kind = ADDRESS;
            icodes5 = gen_assign_ir(ASSIGN, place, t6);
        }
    else 
        icodes5 = gen_assign_ir(ASSIGN, place, t4);

    icodes = codeBind(icodes, icodes5);
    return icodes;
}

InterCodes structures(struct Node *node, Operand place, int left) {
    struct Node *child = node->arity;
    enum KType x;
    int offset;
    assert(child != NULL);
    InterCodes icodes, icodes1, icodes2;
    //printf("in structures\n");
    if (strcmp(child->type, "ID") == 0) {
        int a = lookup_stable(child->text, &x);
        //struct SymbolMsg *sss = getVar(child->text);
        char *fiename;
        struct SymbolMsg *sMsg = getVar(child->text);
        //printf("first %d \n", sMsg->isParam);
        //printf("*******\n");
        struct SymbolMsg *p;
        p = getVar(sMsg->strtype);
        //printf("%s \n", p->name);
        //printf("strtype :)))) %s \n", sMsg->strtype);
        struct FieldMsg *fie = p->fieldList;
        printf("@@@@@@@@\n");
        //if (fie == NULL) printf("!!!!!\n");
        fiename = node->nextsibling->nextsibling->text;
        offset = 0;
        int time = 0;
        while (fie != NULL) {
            time = time + 1;
            if (strcmp(fie->name, fiename) == 0) break;
            struct SymbolMsg *ss = getVar(fie->name);
            offset = offset + cal_offset(ss);
            fie = fie->nextField;
        }
        //printf("%d\n", time);
        Operand c = new_const(offset);
        Operand t1 = new_ref(a);
        //printf("@@@@@@@@\n");
        //sssMsg = getVar(child->text);
        //printf("second %d \n", sMsg->isParam);
        //printf("str no:  %s %d %d\n", child->text, sMsg->isParam,  a);
       // if (sMsg->isParam == 1) printf("is param!!!!\n");
        if (sMsg->isParam == 1) t1->kind = VARIABLE;
        //if (t1->isParam == 1) 
        Operand t2 = new_tmp();
        Operand t3 = new_tmp();
        icodes1 = gen_binop_ir(ADD, t2, t1, c);
        if (left == 0) 
            {
                t3->u.tmp_no = t2->u.tmp_no;
                t3->kind = ADDRESS;
                icodes2 = gen_assign_ir(ASSIGN, place, t3);
            }
        else icodes2 = gen_assign_ir(ASSIGN, place, t2);
        icodes = codeBind(icodes1, icodes2);
        return icodes;
    }
    else if (strcmp(child->arity->nextsibling->nextsibling->type, "LB") == 0) {
    }
    return NULL;
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
                fputs("LABEL ", fp);
                //printf("########%x\n", (unsigned int)ir->code->u.uniop.op);
                print_op(ir->code->u.uniop.op);
                fputs(" :\n", fp);
                break;
            case FUNC_DEF:
                fputs("FUNCTION ", fp);
                print_op(ir->code->u.uniop.op);
                fputs(" :\n", fp);
                break;
            case ASSIGN:
                print_op(ir->code->u.assign.left);
                fputs(" := ", fp);
                print_op(ir->code->u.assign.right);
                fputs("\n", fp);
                break;
            case ADD:
                print_op(ir->code->u.binop.result);
                fputs(" := ", fp);
                print_op(ir->code->u.binop.op1);
                fputs(" + ", fp);
                print_op(ir->code->u.binop.op2);
                fputs("\n", fp);
                break;
            case SUB: 
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
                break;
            case DIV:
                print_op(ir->code->u.binop.result);
                fputs(" := ", fp);
                print_op(ir->code->u.binop.op1);
                fputs(" / ", fp);
                print_op(ir->code->u.binop.op2);
                fputs("\n", fp);
                break;
            case LABEL_GOTO:
                fputs("GOTO ", fp);
                print_op(ir->code->u.uniop.op);
                fputs("\n", fp);
                break;
            case LABEL_REL:
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
                break;
            case RETURN:
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
                fputs("ARG ", fp);
                print_op(ir->code->u.uniop.op);
                fputs("\n", fp);
                break;
            case CALLFUNC:
                print_op(ir->code->u.assign.left);
                fputs(" := CALL ", fp);
                print_op(ir->code->u.assign.right);
                fputs("\n", fp);
                break;
            case I_PARAM:
                fputs("PARAM ", fp);
                //ir->code->u.uniop.op->kind = VARIABLE;
                print_op(ir->code->u.uniop.op);
                fputs("\n", fp);
                break;
            case READ:
                fputs("READ ", fp);
                print_op(ir->code->u.uniop.op);
                fputs("\n", fp);
                break;
            case WRITE:
                fputs("WRITE ", fp);
                print_op(ir->code->u.uniop.op);
                fputs("\n", fp);
                break;
            
        }
        ir = ir->next;
        //if (ir == NULL) printf("ir is null\n");
    }
    fclose(fp);
}

void print_op(Operand op) {
    //printf("!!!!in print_op\n");
    //if (op == NULL) { printf("!!!\n"); return; }
    switch (op->kind)
    {
        case VARIABLE: //printf("v%d\n", op->u.var_no); 
                       fprintf(fp, "v%d", op->u.var_no); break;
        case CONSTANT: fprintf(fp, "#%d", op->u.value); break;
        case PARAM: fprintf(fp, "%s", op->u.pname); break;
        case LABEL: //printf("in op label\n"); 
                    fprintf(fp, "label%d", op->u.lab_no); break;
        case FUNCTION: fprintf(fp, "%s", op->u.fname); break;
        case TEMP: printf("t%d\n", op->u.tmp_no); fprintf(fp, "t%d", op->u.tmp_no); break;
        case REFERENCE: fprintf(fp, "&v%d", op->u.addr_no); break;
        case ADDRESS: fprintf(fp, "*t%d", op->u.tmp_no); break;
    }
    return;
} 
