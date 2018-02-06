/*************************************************************************
	> File Name: semantic.c
	> Author: 
	> Mail: 
	> Created Time: Fri 22 Apr 2016 04:30:57 PM CST
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"node.h"
#include"semantic.h"
#define tablesize 30000


struct SymbolMsg *symbolTable[tablesize];
struct FuncMsg *funcTable[tablesize];
extern struct Node *root;

void semantic()
{
    init();
    //printf("%d\n", hash_pjw(""));
    Program_t(root);
    for (int i = 0; i < tablesize; i++) 
    {
        struct FuncMsg *tmp = funcTable[i];
        if (tmp != NULL)
        {
            //printf("%s %d %d %s",tmp->name, tmp->symType, tmp->isArray, tmp->strtype);
            //if (tmp->fieldList!=NULL) printf(" !");
            //printf("\n");
            /*if (tmp->arr != NULL)
            {
                ap = tmp->arr;
                while (ap->nextarrType!= NULL) 
                {  
                    ap = ap->nextarrType;
                }
            }*/
            if (tmp->isDef == 0) printf("Error Type 18 at Line %d: Undefined function '%s'.\n", tmp->lineNumber, tmp->name);
            while (tmp->nextFunc != NULL)
            {
                tmp = tmp->nextFunc;
            }
        }
    }
    /*for (int i = 0; i < tablesize; i++) 
    {
        struct SymbolMsg *tmp = symbolTable[i];
        struct ArrMsg *ap;
        if (tmp != NULL)
        {
            printf("%s %d %d %s %d",tmp->name, tmp->symType, tmp->isArray, tmp->strtype, tmp->inStr);
            if (tmp->fieldList!=NULL) printf(" !");
            printf("\n");
            if (tmp->arr != NULL)
            {
                ap = tmp->arr;
                while (ap->nextarrType!= NULL) 
                {  
                    ap = ap->nextarrType;
                }
            }
            while (tmp->nextSym!=NULL)
            {
                tmp = tmp->nextSym;
            }
        }
    }*/
}

void init()
{
    memset(symbolTable, 0, sizeof(struct SymbolMsg * )*tablesize);
    memset(funcTable, 0, sizeof(struct FuncMsg * )*tablesize);
}

/* Traversing the tree */
void Program_t(struct Node *node)
{
    if (node == NULL || strcmp(node->type, "Program") != 0){
        printf("The node is not a program.\n");
        return;
    } 
    struct Node *child = node->arity;
    if (child != NULL) ExtDefList_t(child);
}

void ExtDefList_t(struct Node *node)
{
    if (node == NULL || strcmp(node->type, "ExtDefList") != 0)
    {
        printf("The node is not a ExtDefList.\n");
        return;
    }
    struct Node *child = node->arity;   
    if (child != NULL)
    {
        struct SymbolMsg *newstr = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
        //the init of the newstr is here.
        newstr->err = 0;
        ExtDef_t(child, newstr);
        struct Node *bro = child->nextsibling;
        if (bro != NULL) ExtDefList_t(bro);
    }
}

void ExtDef_t(struct Node *node,struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "ExtDef") != 0)
    {
        printf("The node is not a ExtDef.\n");
        return;
    }
    struct Node *child = node->arity;   
    //struct SymbolMsg *newstr = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    //the init of the newstr is here.
    newstr->err = 0;
    newstr->dimension = 0;
    if (child != NULL){
        enum KType GlobalVarType =  Specifier_t(child, newstr);
        if (GlobalVarType == ERROR){
            printf("GType Error.\n");
            return;
        }
        struct Node *bro = child->nextsibling;
        if (bro != NULL)
            if (strcmp(bro->type, "ExtDecList") == 0){
                ExtDecList_t(bro, GlobalVarType, newstr);
            }
            else if (strcmp(bro->type, "FunDec") == 0){
                if (strcmp(bro->nextsibling->type, "CompSt") == 0)
                {    
                    FunDec_t(bro, GlobalVarType, newstr, 1);
                    CompSt_t(bro->nextsibling, GlobalVarType, newstr);
                }
                else    FunDec_t(bro, GlobalVarType, newstr, 0);
            }
            else if (strcmp(bro->type, "SEMI") == 0){
                // do nothing
            }
    }
}

enum KType Specifier_t(struct Node *node, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "Specifier") != 0)
    {
        printf("The node is not a Specifier.\n");
        return ERROR;
    }
    struct Node *child = node->arity;
    if (child != NULL){
        if (strcmp(child->type, "TYPE") == 0)
            if (strcmp(child->text, "int") == 0)
                return 0;
            else if (strcmp(child->text, "float") == 0)
                return 1;
            else return ERROR;
        else if (strcmp(child->type, "StructSpecifier") == 0){
            StructSpecifier_t(child, newstr);
            //if (newstr->fieldList == NULL) printf("in speci !!!!!\n");
            return 3;
        }
    }
}

void ExtDecList_t(struct Node *node, enum KType sType, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "ExtDecList") != 0){
        printf("The node is not a ExtDecList.\n");
        return;
    }
    //printf("in edcl.\n");
    struct Node *child = node->arity;
    if (child != NULL){
        struct ArrMsg *tmp = NULL; 
        VarDec_t(child, sType, tmp, newstr);
        //if (sType == 3) printf("in extdefl\n");
        struct Node *bro = child->nextsibling;
        if (bro != NULL) 
            ExtDecList_t(child->nextsibling->nextsibling, sType, newstr);
        }
}

void StructSpecifier_t(struct Node *node, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "StructSpecifier") != 0){
        printf("The node is not a structspecifier.\n");
        return;
    }
    struct Node *child = node->arity;
    if (strcmp(child->nextsibling->type, "OptTag") == 0){
        newstr->symType = 3;
        newstr->isStr = 1;
        //!!!!!!!!!!!!!!!!***************????????????????
        strcpy(newstr->strtype, child->nextsibling->arity->text); 
        DefList_t(child->nextsibling->nextsibling->nextsibling, newstr);
        OptTag_t(child->nextsibling, newstr); 
        newstr->isStr = 0;
    }
    //  if child->next == LC
    else if (strcmp(child->nextsibling->type, "LC") == 0){
        newstr->symType = 3;
        newstr->isStr = 1;
        strcpy(newstr->strtype, ""); 
        //**************
        DefList_t(child->nextsibling->nextsibling, newstr);
        newstr->isStr = 0;
    }
    else if (strcmp(child->nextsibling->type, "Tag") == 0){
            Tag_t(child->nextsibling, newstr);
        }
}

void OptTag_t(struct Node *node, struct SymbolMsg *newstr)
{
    struct Node *child = node->arity;
    //strcpy(newstr->strtype, child->text);
    //strcpy(newstr->name, child->text);
    if (strcmp(child->type, "ID") == 0){
        //newSymbol(child, 3, NULL, newstr);
        char name[30];
        strcpy(name, child->text);
        unsigned int shashval = hash_pjw(child->text);
        struct SymbolMsg *tmp = symbolTable[shashval];
        while (tmp != NULL){
            if (strcmp(tmp->name, name) == 0 && tmp->strtype!=NULL){ 
                printf("Error Type 16 at Line %d: Duplicated name '%s'.\n", child->line, name);
                return;
            }
            tmp = tmp->nextSym;
            }
        strcpy(newstr->name, name);
        newstr->lineNumber = node->line;
        newstr->symType = 3;
        newstr->isArray = 0;
        newstr->nextSym = NULL;
        //if (sType == 3) 
        //{
            strcpy(newstr->strtype, name);
            newstr->fieldList = newstr->fieldList;
        //}
        newstr->arr = NULL;
    
    if (symbolTable[shashval] == NULL) 
        symbolTable[shashval] = newstr;
    else{
        newstr->nextSym = symbolTable[shashval];
        symbolTable[shashval] = newstr;
    }
    }
}

void Tag_t(struct Node *node, struct SymbolMsg *newstr)
{
    struct Node *child = node->arity;
    enum KType t;
    int k;
    int f = inSymList(child->text, &t, &k);
    if (!f || t != 3) 
        {
            printf("Error Type 17 at Line %d: Undefined structure '%s'.\n", child->line, child->text);
            newstr->err = 1;
            return;
        }
    strcpy(newstr->strtype, child->text);
}

void VarDec_t(struct Node *node, enum KType sType, struct ArrMsg *vArray, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "VarDec") != 0){
        printf("The node is not a VarDec.\n");
        return;
    }
    struct Node *child = node->arity;
    //printf("in VarDec\n");
    if (child != NULL){
        if (strcmp(child->type, "ID") == 0){
            newSymbol(child, sType, vArray, newstr);
        }
        else if (strcmp(child->type, "VarDec") == 0){
            if (vArray == NULL){
                struct ArrMsg *newarr = (struct ArrMsg *)malloc(sizeof(struct ArrMsg));
                newarr->aType = sType;
                newarr->size = atoi(child->nextsibling->nextsibling->text);
                newarr->nextarrType = NULL; 
                vArray = newarr;
                //printf("%d\n", vArray->size);
                VarDec_t(child, sType, vArray, newstr);
            }
            else{
                struct ArrMsg *tmp = vArray;
                struct ArrMsg *newarr = (struct ArrMsg *)malloc(sizeof(struct ArrMsg));
                newarr->aType = 2;
                newarr->size = atoi(child->nextsibling->nextsibling->text);
                newarr->nextarrType = tmp;
                vArray = newarr;
                VarDec_t(child, sType, vArray, newstr);
            }
        }
    }
}

void FunDec_t(struct Node *node, enum KType fType, struct SymbolMsg *newstr, int defed)
{
    if (node == NULL || strcmp(node->type, "FunDec") != 0){
        printf("The node is not a FunDec.\n");
        return;
    }
    struct Node *child = node->arity;
    struct ArgList *args = malloc(sizeof(struct ArgList));
    if (strcmp(child->nextsibling->nextsibling->type, "VarList") == 0 && defed == 1){
        args = VarList_t(child->nextsibling->nextsibling, newstr);
    }
    else args = NULL;    // the function does not have any parameters
    newFunc(child, args, fType, defed);    
}

struct ArgList *VarList_t(struct Node *node, struct SymbolMsg *newstr)
{ 
    if (node == NULL || strcmp(node->type, "VarList") != 0){
        printf("The node is not a VarList.\n");
        return NULL;
    }
    struct Node *child = node->arity;
    struct ArgList *p = (struct ArgList *)malloc(sizeof(struct ArgList));
    struct ArgList *tmp = ParaDec_t(child, newstr);
    p->nextarg = NULL;
    strcpy(p->name, tmp->name);
    p->argType = tmp->argType;
    //printf("in varlist %d\n", p->argType);
    if (child->nextsibling != NULL){
        p->nextarg = VarList_t(child->nextsibling->nextsibling, newstr);
    }
    return p;
}

struct ArgList *ParaDec_t(struct Node *node, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "ParamDec") != 0){
        printf("The node is not a ParamDec.\n");
        return NULL;
    }
    struct Node *child = node->arity;
    struct ArgList *tp = (struct ArgList *)malloc(sizeof(struct ArgList));
    struct SymbolMsg *x = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    //**********
    enum KType argtype = Specifier_t(child, x);
    //printf("in paradec.\n");
    if (argtype == ERROR) 
        {
            printf("ParaType Error.\n");
            return NULL;
        }
    struct Node *bro = child->nextsibling;
    struct ArrMsg *tmp = NULL;
    strcpy(tp->name, bro->arity->text);
    tp->argType = argtype;
    VarDec_t(child->nextsibling, argtype, tmp, x);  
    return tp;
}

void CompSt_t(struct Node *node, enum KType fType, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "CompSt") != 0){
        printf("The node is not a CompSt.\n");
        return;
    }
    struct Node *child = node->arity;
    struct Node *bro = child->nextsibling;
    //***********
    struct SymbolMsg *x = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    if (bro != NULL)
        if (strcmp(bro->type, "DefList") == 0){
            DefList_t(bro, x);
            bro = bro->nextsibling;
        }
    if (bro != NULL){
        if (strcmp(bro->type, "StmtList") == 0){
            StmtList_t(bro, fType, newstr);
        }
    }
}

void DefList_t(struct Node *node, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "DefList") != 0){
        printf("The node is not a DefList at line %d.\n", node->line);
        return;
    }
    struct Node *child = node->arity;
    if (child != NULL){
        Def_t(child, newstr);
        if (child->nextsibling != NULL){
            DefList_t(child->nextsibling, newstr);
        }
    }
}
 
void StmtList_t(struct Node *node, enum KType fType, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "StmtList") != 0){
        printf("The node is not a StmtList.\n");
        return;
    }
    struct Node *child = node->arity;
    if (child != NULL){
        Stmt_t(child, fType, newstr);
        if (child->nextsibling != NULL){
            StmtList_t(child->nextsibling, fType, newstr);
        }
    }
}

void Stmt_t(struct Node *node, enum KType fType, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "Stmt") != 0){
        printf("The node is not a Stmt.\n");
        return;
    }
    struct Node *child = node->arity;
    struct SymbolMsg *mes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    struct FuncMsg *mes2 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
    enum KType eType, e;
    if (strcmp(child->type, "Exp") == 0){
        Exp_t(child, mes1, mes2, newstr);                         
    }
    else if (strcmp(child->type, "CompSt") == 0){
        CompSt_t(child, fType, newstr);                     
    }
    else if (strcmp(child->type, "RETURN") == 0){
        Exp_t(child->nextsibling, mes1, mes2, newstr);    
        if (mes1->symType == 4 && mes2->rtType != 4) { e = mes2->rtType; }
        if (mes1->symType != 4 && mes2->rtType == 4) { e = mes1->symType; } 
        if (e != fType) printf("Error Type 8 at Line %d: Type mismatched for return.\n", child->line);
    }
    else if (strcmp(child->type, "IF") == 0){
        struct Node *bro = child->nextsibling->nextsibling;
        Exp_t(bro, mes1, mes2, newstr);                         
        bro = bro->nextsibling->nextsibling;
        Stmt_t(bro, fType, newstr);
        bro = bro->nextsibling;
        if (bro != NULL && strcmp(bro->type, "ELSE")==0){
            bro = bro->nextsibling;
            Stmt_t(bro, fType, newstr);
        }
    }
    else if (strcmp(child->type, "WHILE") == 0){
        struct Node *bro = child->nextsibling->nextsibling;
        Exp_t(bro, mes1, mes2, newstr);                     
        bro = bro->nextsibling->nextsibling;
        Stmt_t(bro, fType, newstr);
        bro = bro->nextsibling;
    }
}

void Def_t(struct Node *node, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "Def") != 0){
        printf("The node is not a Def.\n");
        return;
    }
    struct Node *child = node->arity;
    if (strcmp(child->type, "Specifier") == 0){
        enum KType LocalVarType = Specifier_t(child, newstr);
        if (LocalVarType == ERROR){
            printf("LType Error.\n");
            return;
        }
        struct Node *bro = child->nextsibling;
        DecList_t(bro, LocalVarType, newstr);
    }
}

void DecList_t(struct Node *node, enum KType LocalVarType, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "DecList") != 0){
        printf("The node is not a DecList.\n");
        return;
    }
    struct Node *child = node->arity;
    Dec_t(child, LocalVarType, newstr);
    if (child->nextsibling != NULL){
        DecList_t(child->nextsibling->nextsibling, LocalVarType, newstr);
    }
}

void Dec_t(struct Node *node, enum KType LocalVarType, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "Dec") != 0){
        printf("The node is not a Dec.\n");
        return;
    }
    //printf("in dec\n");
    struct Node *child = node->arity;
    struct SymbolMsg *mes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    struct FuncMsg *mes2 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
    struct ArrMsg *tmp = NULL;
    VarDec_t(child, LocalVarType, tmp, newstr);
    enum KType eType;
    if (child->nextsibling != NULL){
        if (newstr->isStr == 1) printf("Error Type 15 at Line %d: Cannot initialize a member in struct.\n", child->line);
        Exp_t(child->nextsibling->nextsibling, mes1, mes2, newstr);
    }
}

unsigned int hash_pjw(char* name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff) 
            val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

void Exp_t(struct Node *node, struct SymbolMsg *SMes, struct FuncMsg *FMes, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "Exp") != 0){
        printf("The node is not a Exp at line %d.\n", node->line);
        return;
    }
    struct Node *child = node->arity;
    struct ArrMsg *ap;
    struct SymbolMsg *smes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    struct FuncMsg *fmes1 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
    struct SymbolMsg *smes2 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    struct FuncMsg *fmes2 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
    enum KType e1, e2;
    struct FieldMsg *tp1 = (struct FieldMsg *)malloc(sizeof(struct FieldMsg));
    struct FieldMsg *tp2 = (struct FieldMsg *)malloc(sizeof(struct FieldMsg));
    int f, f1, f2, i, s1, s2;
    char *c;
    enum KType e;
    //in case not defined 
    FMes->rtType = 4;
    SMes->symType = 4;
    SMes->isArray = 0;
    if (strcmp(child->type, "LP") == 0){
        Exp_t(child->nextsibling,smes1, fmes1, newstr);
        if (smes1->symType == 4 && fmes1->rtType != 4) { e = fmes1->rtType; }
        if (smes1->symType != 4 && fmes1->rtType == 4) { e = smes1->symType; }
        SMes->symType = e;
    }
    if (strcmp(child->type, "MINUS") == 0 || strcmp(child->type, "NOT") == 0){
        int r = 0;
        Exp_t(child->nextsibling,smes1, fmes1, newstr);
        if (smes1->symType == 4 && fmes1->rtType != 4) { e = fmes1->rtType; r = 0;}
        if (smes1->symType != 4 && fmes1->rtType == 4) { e = smes1->symType; r = smes1->isArray;}
        if (e == 1 || e == 0 && r == 1) printf("Error Type 7. at Line %d: Type mismatched for operands.\n", child->line);
        else if (e != 1 && e != 0) printf("Error Type 7 at Line %d: Type mismatched for operands.\n", child->line);
        else SMes->symType = e;
    }
    if (strcmp(child->type, "ID") == 0)
    {
        struct FuncMsg *tp;
        struct ArgList *tpa;
        if (child->nextsibling == NULL)    // ID alone
        {                                       
            f = 0;
            unsigned int hashval = hash_pjw(child->text);
            struct SymbolMsg *tmp = symbolTable[hashval];
            while (tmp != NULL)
            {
                if (strcmp(tmp->name, child->text) == 0){
                    f = 1;
                    //*************
                    //printf("%s\n", child->text);
                    SMes->symType = tmp->symType;
                    /*switch (tmp->symType){
                        case 0: SMes->symType = INT; break;
                        case 1: SMes->symType = FLOAT; break;
                        case 
                    }*/
                    SMes->isArray = tmp->isArray;
                    SMes->dimension = tmp->dimension;
                    SMes->lineNumber = tmp->lineNumber;
                    SMes->inStr = tmp->inStr;
                    //printf("%s %d\n", tmp->name, tmp->inStr); 
                    SMes->arr = tmp->arr;
                    //SMes->fieldList = tmp->fieldList;
                    //if (tmp->fieldList == NULL) printf("in exp !!!\n");
                    strcpy(SMes->strtype, tmp->strtype);
                    //printf("%s !!!\n", SMes->strtype);
                    unsigned int ha = hash_pjw(SMes->strtype);
                    struct SymbolMsg *txp = symbolTable[ha];
                    while (txp != NULL){
                        if (strcmp(txp->name, SMes->strtype) == 0)
                            {SMes->fieldList = txp->fieldList; break;}
                        txp = txp->nextSym;
                    }
                    strcpy(SMes->name, tmp->name);
                    break;
                }
                tmp = tmp->nextSym;
            }
            if (!f) printf("Error Type 1 at Line %d: Undefined variable '%s'.\n", child->line, child->text);
        }
        else    //function 
        {
            enum KType xType;
            int l;
            f = inSymList(child->text, &xType, &l);
            if (f) printf("Error Type 11 at Line %d: '%s' is not a function.\n", child->line, child->text);
            else
            {
                unsigned int fhashval = hash_pjw(child->text);
                struct FuncMsg *tmp = funcTable[fhashval];
                f = 0;
                while (tmp != NULL)
                {
                    if (strcmp(tmp->name, child->text) == 0){
                        f = 1;
                        //************
                        //printf("in exp funmes %d\n", tmp->rtType);
                        FMes->rtType = tmp->rtType;
                        /*switch (tmp->rtType){
                            case 0: FMes->rtType = INT; break;
                            case 1: FMes->rtType = FLOAT; break;
                        }*/
                        //printf("in exp funmes %d\n", FMes->rtType);
                        FMes->isArray = tmp->isArray;
                        FMes->argnum = tmp->argnum;
                        FMes->lineNumber = tmp->lineNumber;
                        FMes->isDef = tmp->isDef;
                        FMes->dimension = FMes->dimension;
                        strcpy(FMes->strtype, tmp->strtype);
                        strcpy(FMes->name, tmp->name);
                        tp = tmp;                    //tp is the one in the FuncList
                        tpa = tp->Args;              //tpa is one of its attributes
                        break;
                    }
                    tmp = tmp->nextFunc;
                }
                if (!f) {
                    printf("Error Type 2 at Line %d: Undefined function '%s'.\n", child->line, child->text);  
                    return;
                }
                f1 = 1;
                if (strcmp(child->nextsibling->nextsibling->type, "Args") == 0){
                    struct ArgList *args = (struct ArgList *)malloc(sizeof(struct ArgList));
                    struct ArgList *tpat = tpa;
                    args = Args_t(child->nextsibling->nextsibling, newstr); 
                    int num = 0;
                    struct ArgList *pp = args;
                    while (pp != NULL)
                    {
                        num++;
                        pp = pp->nextarg;
                    }
                    if (FMes->argnum != num){
                        f1 = 0;
                    }
                    else{
                        pp = args;                 //pp is the arglist for an func 
                        while (pp != NULL && tpa!=NULL)
                        {
                            //printf("prev %d   %d\n", tpa->argType, pp->argType);
                            if (tpa->argType != pp->argType) f1 = 0;
                            pp = pp->nextarg;
                            tpa = tpa->nextarg;
                        }
                    }
                    if (!f1) 
                    {
                        printf("Error Type 9 at Line %d: Function '%s(", child->line, FMes->name);
                        for (i = 1; i <= FMes->argnum; i++){
                            if (i != FMes->argnum){
                                switch (tpat->argType){
                                    case 0: printf("int, "); break;
                                    case 1: printf("float, "); break;
                                }
                            }
                            else{
                                switch (tpat->argType){
                                    case 0: printf("int)' "); break;
                                    case 1: printf("float)'"); break;
                                }
                            } 
                            tpat = tpat->nextarg;
                        }
                        printf("is not applicable for arguments '(");
                        pp = args;
                        for (i = 1; i <= num; i++){
                            if (i != num){
                                switch (pp->argType){
                                    case 0: printf("int, "); break;
                                    case 1: printf("float, "); break;
                                    case 2: printf("array, "); break;
                                }
                            }
                            else{
                                switch (pp->argType){
                                    case 0: printf("int)'.\n"); break;
                                    case 1: printf("float)'.\n"); break;
                                    case 2: printf("array)'.\n"); break;
                                }
                            }
                            pp = pp->nextarg; 
                        }
                    }
                }   
            }
        }
    }
    else if (strcmp(child->type, "INT") == 0)
    {
        //printf("!!!!\n");
        SMes->symType = 0;
        strcpy(SMes->name, child->text);
    }
    else if (strcmp(child->type, "FLOAT") == 0)
    {
        SMes->symType = 1;
        strcpy(SMes->name, child->text);
    }
    else if (strcmp(child->type, "Exp") == 0)
    {
        struct Node *bro = child->nextsibling;
        if (strcmp(bro->type, "ASSIGNOP") == 0){
            //printf("!!!!!\n");
            f = 1;
            if (strcmp(child->arity->type, "ID") == 0  && child->arity->nextsibling == NULL) f = 0;
            else if (strcmp(child->arity->type, "Exp") == 0)
                if (strcmp(child->arity->nextsibling->type, "LB") == 0)
                    f = 0;
                else if (strcmp(child->arity->nextsibling->type, "DOT") == 0)
                    f = 0;
            if (f) printf("Error Type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", child->line);
            //struct SymbolMsg *smes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
            //struct SymbolMsg *smes2 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));  
            //struct FuncMsg *fmes1 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
            //struct FuncMsg *fmes2 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
            f = 0;
            //************
            Exp_t(child, smes1, fmes1, newstr);
            Exp_t(bro->nextsibling, smes2, fmes2, newstr);
            /*if (smes1->symType == 4 && fmes1->rtType != 4) {f1 = fmes1->rtType; f2 = fmes1->dimension;}
            if (smes1->symType != 4 && fmes1->rtType == 4) {f1 = smes1->symType; f2 = smes1->dimension;}
            if (smes1->symType != smes2->symType) f = 0; 
            else if (smes1->dimension != smes2->dimension) f = 0;*/
            if (smes1->symType == 4 && fmes1->rtType != 4) { e1 = fmes1->rtType; s1 = 0; }
            if (smes1->symType != 4 && fmes1->rtType == 4) { e1 = smes1->symType; s1 = smes1->isArray; f1 = smes1->dimension; tp1 = smes1->fieldList; } 
            if (smes2->symType == 4 && fmes2->rtType != 4) { e2 = fmes2->rtType; s2 = 0; }
            if (smes2->symType != 4 && fmes2->rtType == 4) { e2 = smes2->symType; s2 = smes2->isArray; f2 = smes2->dimension; tp2 = smes2->fieldList; }
            //printf("e1: %d  %d\n", e1, s1);
            //printf("e2: %d  %d\n", e2, s2);
            if (e1 == 3 && e2 == 3){
                //printf("%d  %d\n", e1, e2);
                while (tp1!=NULL && tp2!=NULL)
                 {
                    //printf("%d  %d\n", tp1->fieldType, tp2->fieldType);
                    if  (tp1->fieldType != tp2->fieldType) { f = 1; break; }
                    tp1 = tp1->nextField;
                    tp2 = tp2->nextField;
                     if (tp1 == NULL && tp2 != NULL || tp1 != NULL && tp2 == NULL) f = 1;
                 }
             }
            else if (e1 != e2) f = 1;
            else if (s1 == 1 && s2 != 1 || s1 != 1 && s2 == 1) f = 1;
            else if (s1 == 1 && s2 == 1 && f1 != f2) f = 1;
            if (f) printf("Error Type 5 at Line %d: Type mismatched for assignment.\n", child->line);
        }
        else if (strcmp(bro->type, "LB") == 0)
        {
            Exp_t(child, smes1, fmes1, newstr);
            if (smes1->arr == NULL) printf("Error Type 10 at Line %d: '%s' is not an array.\n", child->line, smes1->name);
            Exp_t(child, smes1, fmes1, newstr);
            SMes->symType = smes1->symType;
            Exp_t(child->nextsibling->nextsibling, smes1, fmes1, newstr);
            if (smes1->symType != 0) printf("Error Type 12 at Line %d: '%s' is not an integer\n", child->line, smes1->name);
        }
        else if (strcmp(bro->type, "DOT") == 0){
                enum KType y;
                int k;
                Exp_t(child, smes1, fmes1, newstr);
                if (smes1->symType == 4 && fmes1->rtType != 4) c = fmes1->name;  
                if (smes1->symType != 4 && fmes1->rtType == 4) c = smes1->name; 
                //printf("%d  %d\n", smes1->symType, fmes1->rtType);
                f = inSymList(c, &y, &k);
                //printf("in exp dot %d\n", k);
                if (f && !k) { printf("Error Type 13 at Line %d: Illigal use of '.'.\n", child->line);return;}
                else{
                    //printf("%d  %d\n", smes1->symType, fmes1->rtType);
                    //printf("in exp dot\n");
                    if (smes1->symType == 4 && fmes1->rtType != 4) c = fmes1->strtype;
                    if (smes1->symType != 4 && fmes1->rtType == 4) c = smes1->strtype;
                    if (strcmp(c, "") != 0){
                        unsigned int h = hash_pjw(c);
                        //printf("%d\n", h);
                        struct SymbolMsg *t = symbolTable[h];
                        while (t != NULL)
                        {
                            if (strcmp(c, t->name) == 0){
                                f = 1;
                                break;
                            }
                        t = t->nextSym;
                        }
                        struct FieldMsg *fi = t->fieldList;
                        f = 0;
                        while (fi != NULL)
                        {
                            if (strcmp(fi->name, bro->nextsibling->text) == 0) { f = 1; e = fi->fieldType;}
                            fi = fi->nextField;
                        }
                        if (!f) {printf("Error Type 14 Line %d: Non-existent field '%s'.\n", child->line, bro->nextsibling->text); return; }
                    }
                    else{ int fff, sk;   
                          enum KType sx;
                          fff = inSymList(bro->nextsibling->text, &sx, &sk);
                          if (fff) e = sx; else {printf("Error Type 14 at Line %d: Non-existence field '%s'.\n", child->line, bro->nextsibling->text);return; }
                        }
                    SMes->symType = e;
                }
        }
        else if (strcmp(bro->type, "AND") == 0 || strcmp(bro->type, "OR") == 0){
                f = 0;
                enum KType e1, e2;
                Exp_t(child, smes1, fmes1, newstr);
                Exp_t(child->nextsibling->nextsibling, smes2, fmes2, newstr);
                if (smes1->symType == 4 && fmes1->rtType != 4) e1 = fmes1->rtType;
                if (smes1->symType != 4 && fmes1->rtType == 4) e1 = smes1->symType;
                if (smes2->symType == 4 && fmes2->rtType != 4) e2 = fmes2->rtType;
                if (smes2->symType != 4 && fmes2->rtType == 4) e2 = smes2->symType;
                if (e1 != 0 || e2 != 0) printf("Error Type 7 at Line %d: Type mismatched for operands.\n", child->line);
                else SMes->symType = 0;
        }
        else{    //op:relop plus minus star div 
            //int s1, s2;
            f = 0;
            //enum KType e1, e2;
            //struct FieldMsg *tp1 = (struct FieldMsg *)malloc(sizeof(struct FieldMsg));
            //struct FieldMsg *tp2 = (struct FieldMsg *)malloc(sizeof(struct FieldMsg));
            Exp_t(child, smes1, fmes1, newstr);
            Exp_t(child->nextsibling->nextsibling, smes2, fmes2, newstr);
            //************
            //if (smes1->symType == 4 && fmes1->rtType != 4) {f1 = fmes1->rtType; f2 = fmes1->dimension;}
            //if (smes1->symType != 4 && fmes1->rtType == 4) {f1 = smes1->symType; f2 = smes1->dimension;}
            if (smes1->symType == 4 && fmes1->rtType != 4) { e1 = fmes1->rtType; s1 = 0; }
            if (smes1->symType != 4 && fmes1->rtType == 4) { e1 = smes1->symType; s1 = smes1->isArray; f1 = smes1->dimension; tp1 = smes1->fieldList; } 
            if (smes2->symType == 4 && fmes2->rtType != 4) { e2 = fmes2->rtType; s2 = 0;}
            if (smes2->symType != 4 && fmes2->rtType == 4) { e2 = smes2->symType; s2 = smes2->isArray; f2 = smes2->dimension; tp2 = smes2->fieldList; }
            if (e1 == 3 && e2 == 3){
                //printf("%d  %d\n", e1, e2);
                while (tp1!=NULL && tp2!=NULL)
                 {
                    //printf("%d  %d\n", tp1->fieldType, tp2->fieldType);
                    if  (tp1->fieldType != tp2->fieldType) { f = 1; break; }
                    tp1 = tp1->nextField;
                    tp2 = tp2->nextField;
                     if (tp1 == NULL && tp2 != NULL || tp1 != NULL && tp2 == NULL) f = 1;
                 }
             }
            else if (e1 != e2) f = 1;
            else if (s1 == 1 && s2 != 1 || s1 != 1 && s2 == 1) f = 1;
            else if (s1 == 1 && s2 == 1 && f1 != f2) f = 1;
            if (f) printf("Error Type 7 at Line %d: Type mismatched for operands.\n", child->line);
            else SMes->symType = e1;
        }
    }
}

struct ArgList *Args_t(struct Node *node, struct SymbolMsg *newstr)
{
    if (node == NULL || strcmp(node->type, "Args") != 0){
        printf("The node is not a Args.\n");
        return NULL;
    }
    struct SymbolMsg *smes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    struct FuncMsg *fmes1 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
    struct Node *child = node->arity;
    struct ArgList *p = (struct ArgList *)malloc(sizeof(struct ArgList));
    Exp_t(child, smes1, fmes1, newstr);
    //**********
    if (smes1->symType == 4 && fmes1->rtType != 4) p->argType = fmes1->rtType;
    if (smes1->symType != 4 && fmes1->rtType == 4) p->argType = smes1->symType;
    //printf("in args %d\n", smes1->symType);
    p->nextarg = NULL;
    if (child->nextsibling != NULL){
        p->nextarg = Args_t(child->nextsibling->nextsibling, newstr);
    }
    return p;
}

void newSymbol(struct Node *node, enum KType sType, struct ArrMsg *vArray, struct SymbolMsg *newstr)
{
    if (newstr->err) return;
    unsigned int hashval = hash_pjw(node->text);
    //printf("value: %d\n", hashval);
    struct SymbolMsg *tmp = symbolTable[hashval];
    int ff = 0;
    //printf("in newsymbol %s  %d  %s\n", node->text, newstr->isStr, newstr->strtype);
    if (newstr->isStr == 1) 
    {
        ff = 1;
        while (tmp != NULL){
            //printf("%s\n", tmp->name);
            if (strcmp(tmp->name, node->text) == 0 && tmp->inStr == 1 && strcmp(newstr->strtype, tmp->strtype)==0) {
                printf("Error Type 15 at Line %d: Redefined field '%s'.\n", node->line, node->text);
                return;
                }
            tmp = tmp->nextSym;
        }
        struct FieldMsg *tp = (struct FieldMsg *)malloc(sizeof(struct FieldMsg));
        tp->fieldType = sType;
        strcpy(tp->name, node->text);
        tp->nextField = NULL;
        if (newstr->fieldList == NULL){
            newstr->fieldList = (struct FieldMsg *)malloc(sizeof(struct FieldMsg));
            newstr->fieldList = tp;
            //return;
        }
        else{
            struct FieldMsg *p = (struct FieldMsg *)malloc(sizeof(struct FieldMsg));
            p = newstr->fieldList;
            while (p->nextField != NULL) p = p->nextField;
            p->nextField = tp;
        }
    }
    //********
    tmp = symbolTable[hashval];
    while (ff == 0 && tmp != NULL){
        if (strcmp(tmp->name, node->text) == 0){ 
            printf("Error Type 3 at Line %d: Redefined variable '%s'.\n", node->line, node->text);
            return;
        }
        tmp = tmp->nextSym;
    }
    struct SymbolMsg *newsym = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));   
    strcpy(newsym->name, node->text);
    newsym->inStr = ff;
    newsym->lineNumber = node->line;
    newsym->symType = sType;
    newsym->isArray = 0;
    newsym->nextSym = NULL;
    //newsym->fieldList = newstr->fieldList;
    strcpy(newsym->strtype, newstr->strtype);
    /*if (sType == 3) 
    {
        strcpy(newsym->strtype, newstr->strtype);
        newsym->fieldList = newstr->fieldList;
    }*/
    if (vArray != NULL){
        struct ArrMsg *u = (struct ArrMsg *)malloc(sizeof(struct ArrMsg));
        int d = 0;
        u = vArray;
        while (u != NULL) {d++; u = u->nextarrType;}
        //printf("dimension: %d\n", d);
        newsym->dimension = d;
        newsym->arr = vArray;
        newsym->isArray = 1;
    }
    else newsym->arr = NULL;
    
    if (symbolTable[hashval] == NULL) 
        symbolTable[hashval] = newsym;
    else{
        newsym->nextSym = symbolTable[hashval];
        symbolTable[hashval] = newsym;
    }
    /*if (newstr->isStr == 1) 
    {
        struct FieldMsg *tp = (struct FieldMsg *)malloc(sizeof(struct FieldMsg));
        tp->fieldType = sType;
        strcpy(tp->name, node->text);
        tp->nextField = NULL;
        if (newstr->fieldList == NULL){
            newstr->fieldList = (struct FieldMsg *)malloc(sizeof(struct FieldMsg));
            newstr->fieldList = tp;
            return;
        }
        struct FieldMsg *p = (struct FieldMsg *)malloc(sizeof(struct FieldMsg));
        p = newstr->fieldList;
        while (p->nextField != NULL) p = p->nextField;
        p->nextField = tp;
        if (newstr->fieldList == NULL) printf("829484918489\n");
    }*/
}

void newFunc(struct Node *node, struct ArgList *args, enum KType fType, int defed)
{
    unsigned int fhashval = hash_pjw(node->text);
    struct FuncMsg *tmp = funcTable[fhashval];
    while (tmp != NULL){
        if (strcmp(tmp->name, node->text) == 0 && tmp->isDef == 1)
            {printf("Error Type 4 at Line %d: Redefined function '%s'.\n", node->line, node->text); return;}
        if (strcmp(tmp->name, node->text) == 0 && tmp->isDef == 0 && defed == 0)
            {printf("Error Type 19 at Line %d: Inconsistent declaration of function '%s'.\n", node->line, node->text); return;}
        tmp = tmp->nextFunc;
    }
    //printf("in newfunc.\n");
    struct FuncMsg *newfunc = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));   //?
    strcpy(newfunc->name, node->text);
    newfunc->isArray = 0;
    newfunc->isDef = defed;
    newfunc->rtType = fType;
    newfunc->lineNumber = node->line;
    struct ArgList *p = args;
    int num = 0;
    while (p != NULL)
        {
            //printf("%d\n", p->argType);
            num++;
            p = p->nextarg;
        }
    newfunc->argnum = num;
    newfunc->Args = args;
    newfunc->nextFunc = NULL;
    if (funcTable[fhashval] == NULL)
        funcTable[fhashval] = newfunc;
    else{
        newfunc->nextFunc = funcTable[fhashval];
        funcTable[fhashval] = newfunc;
    } 
}

int inSymList(char *text, enum KType *xType, int *k)
{
    unsigned int hashval = hash_pjw(text);
    struct SymbolMsg *tmp = symbolTable[hashval];
    int f = 0;
    //printf("inSymList %s\n", text);
    *k = 0;
    while (tmp != NULL){
        if (strcmp(text, tmp->name) == 0){
            f = 1;
            *xType = tmp->symType;
            if (tmp->symType == 3) *k = 1;
            return f;
            break;
        }
        tmp = tmp->nextSym;
    }
    return 0;
}
/*
int inFuncList(char *text, enum KType *yType)
{
    unsigned int fhashval = hash_pjw(text);
    struct FuncMsg *tmp = funcTable[fhashval];
    int f = 0;
    while (tmp != NULL)
    {
        if (strcmp(tmp->name, child->text) == 0){
            f = 1;
            *yType = tmp->rtType; 
        }
        tmp = tmp->nextFunc;
    }
    if (f) return 1;
    else return 0;
}
*/

