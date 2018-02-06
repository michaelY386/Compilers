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
    Program_t(root);/*
    for (int i = 0; i < tablesize; i++) 
    {
        struct SymbolMsg *tmp = symbolTable[i];
        struct ArrMsg *ap;
        if (tmp != NULL)
        {
            printf("%s %d\n",tmp->name, tmp->symType);
            if (tmp->arr != NULL)
            {
                ap = tmp->arr;
                while (ap->nextarrType!= NULL) 
                {  
                    //printf("@@@@@\n");
                    printf("%d  %d\n", ap->size, ap->aType);
                    ap = ap->nextarrType;
                }
                printf("%d  %d\n", ap->size, ap->aType);
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
        ExtDef_t(child);
        struct Node *bro = child->nextsibling;
        if (bro != NULL) ExtDefList_t(bro);
    }
}

void ExtDef_t(struct Node *node)
{
    if (node == NULL || strcmp(node->type, "ExtDef") != 0)
    {
        printf("The node is not a ExtDef.\n");
        return;
    }
    struct Node *child = node->arity;   
    if (child != NULL) 
    {
        enum KType GlobalVarType =  Specifier_t(child);
        if (GlobalVarType == ERROR){
            printf("GType Error.\n");
            return;
        }
        struct Node *bro = child->nextsibling;
        if (bro != NULL)
            if (strcmp(bro->type, "ExtDecList") == 0){
                ExtDecList_t(bro, GlobalVarType);
            }
            else if (strcmp(bro->type, "FunDec") == 0){
                FunDec_t(bro, GlobalVarType);
                CompSt_t(bro->nextsibling, GlobalVarType);
            }
            else if (strcmp(bro->type, "SEMI") == 0){
                // do nothing
            }
    }
}

enum KType Specifier_t(struct Node *node)
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
                return INT;
            else if (strcmp(child->text, "float") == 0)
                return FLOAT;
            else return ERROR;
        else if (strcmp(child->type, "StructSpecifier") == 0){
            StructSpecifier_t(child);
        }
    }
}

void ExtDecList_t(struct Node *node, enum KType sType)
{
    if (node == NULL || strcmp(node->type, "ExtDecList") != 0){
        printf("The node is not a ExtDecList.\n");
        return;
    }
    struct Node *child = node->arity;
    if (child != NULL){
        struct ArrMsg *tmp = NULL; 
        VarDec_t(child, sType, tmp);
        struct Node *bro = child->nextsibling;
        if (bro != NULL) 
            ExtDecList_t(child->nextsibling->nextsibling, sType);
        }
}
        
void VarDec_t(struct Node *node, enum KType sType, struct ArrMsg *vArray)
{
    if (node == NULL || strcmp(node->type, "VarDec") != 0){
        printf("The node is not a VarDec.\n");
        return;
    }
    struct Node *child = node->arity; 
    if (child != NULL){
        if (strcmp(child->type, "ID") == 0){
            newSymbol(child, sType, vArray);
        }
        else if (strcmp(child->type, "VarDec") == 0){
            if (vArray == NULL){
                struct ArrMsg *newarr = (struct ArrMsg *)malloc(sizeof(struct ArrMsg));
                newarr->aType = sType;
                newarr->size = atoi(child->nextsibling->nextsibling->text);
                newarr->nextarrType = NULL; 
                vArray = newarr;
                VarDec_t(child, sType, vArray);
            }
            else{
                struct ArrMsg *tmp = vArray;
                struct ArrMsg *newarr = (struct ArrMsg *)malloc(sizeof(struct ArrMsg));
                newarr->aType = 3;
                newarr->size = atoi(child->nextsibling->nextsibling->text);
                newarr->nextarrType = tmp;
                vArray = newarr;
                VarDec_t(child, sType, vArray);
            }
        }
    }
}

void FunDec_t(struct Node *node, enum KType fType)
{
    if (node == NULL || strcmp(node->type, "FunDec") != 0){
        printf("The node is not a FunDec.\n");
        return;
    }
    struct Node *child = node->arity;
    struct ArgList *args = malloc(sizeof(struct ArgList));
    if (strcmp(child->nextsibling->nextsibling->type, "VarList") == 0){
        args = VarList_t(child->nextsibling->nextsibling);
    }
    else args = NULL;    // the function does not have any parameters
    newFunc(child, args, fType);    
}

struct ArgList *VarList_t(struct Node *node)
{
    
    if (node == NULL || strcmp(node->type, "VarList") != 0){
        printf("The node is not a VarList.\n");
        return NULL;
    }
    //printf("in arglist.\n");
    struct Node *child = node->arity;
    struct ArgList *p = (struct ArgList *)malloc(sizeof(struct ArgList));
    struct ArgList *tmp = ParaDec_t(child);
    p->nextarg = NULL;
    strcpy(p->name, tmp->name);
    p->argType = tmp->argType;
    if (child->nextsibling != NULL){
        p->nextarg = VarList_t(child->nextsibling->nextsibling);
    }
    return p;
}

struct ArgList *ParaDec_t(struct Node *node)
{
    if (node == NULL || strcmp(node->type, "ParamDec") != 0){
        printf("The node is not a ParamDec.\n");
        return NULL;
    }
    struct Node *child = node->arity;
    struct ArgList *tp = (struct ArgList *)malloc(sizeof(struct ArgList));
    enum KType argtype = Specifier_t(child);
    //printf("in paradec.\n");
    if (argtype == ERROR) 
        {
            printf("ParaType Error.\n");
            return NULL;
        }
    struct Node *bro = child->nextsibling;
    struct ArrMsg *tmp = NULL;
    //printf("%s\n", bro->arity->text);
    //printf("%s\n", tp->name);
    strcpy(tp->name, bro->arity->text);
    //printf("in para.\n");
    tp->argType = argtype;
    //printf("in paradec.\n");
    VarDec_t(child->nextsibling, argtype, tmp);  
    return tp;
}

void CompSt_t(struct Node *node, enum KType fType)
{
    if (node == NULL || strcmp(node->type, "CompSt") != 0){
        printf("The node is not a CompSt.\n");
        return;
    }
    struct Node *child = node->arity;
    struct Node *bro = child->nextsibling;
    if (bro != NULL)
        if (strcmp(bro->type, "DefList") == 0){
            DefList_t(bro);
            bro = bro->nextsibling;
        }
    if (bro != NULL){
        if (strcmp(bro->type, "StmtList") == 0){
            StmtList_t(bro, fType);
        }
    }
}

void DefList_t(struct Node *node)
{
    if (node == NULL || strcmp(node->type, "DefList") != 0){
        printf("The node is not a DefList.\n");
        return;
    }
    struct Node *child = node->arity;
    if (child != NULL){
        Def_t(child);
        if (child->nextsibling != NULL){
            DefList_t(child->nextsibling);
        }
    }
}
 
void StmtList_t(struct Node *node, enum KType fType)
{
    if (node == NULL || strcmp(node->type, "StmtList") != 0){
        printf("The node is not a StmtList.\n");
        return;
    }
    struct Node *child = node->arity;
    if (child != NULL){
        Stmt_t(child, fType);
        if (child->nextsibling != NULL){
            StmtList_t(child->nextsibling, fType);
        }
    }
}

void Stmt_t(struct Node *node, enum KType fType)
{
    if (node == NULL || strcmp(node->type, "Stmt") != 0){
        printf("The node is not a Stmt.\n");
        return;
    }
    struct Node *child = node->arity;
    struct SymbolMsg *mes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    struct FuncMsg *mes2 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
    enum KType eType;
    if (strcmp(child->type, "Exp") == 0){
        Exp_t(child, mes1, mes2);                         
    }
    else if (strcmp(child->type, "CompSt") == 0){
        CompSt_t(child, fType);                     
    }
    else if (strcmp(child->type, "RETURN") == 0){
        Exp_t(child->nextsibling, mes1, mes2);    
        if (mes1->symType != fType) printf("Error Type 8 at Line %d: Type mismatched for return.\n", child->line);
    }
    else if (strcmp(child->type, "IF") == 0){
        struct Node *bro = child->nextsibling->nextsibling;
        Exp_t(bro, mes1, mes2);                         
        bro = bro->nextsibling->nextsibling;
        Stmt_t(bro, fType);
        bro = bro->nextsibling;
        if (bro != NULL && strcmp(bro->type, "ELSE")==0){
            bro = bro->nextsibling;
            Stmt_t(bro, fType);
        }
    }
    else if (strcmp(child->type, "WHILE") == 0){
        struct Node *bro = child->nextsibling->nextsibling;
        Exp_t(bro, mes1, mes2);                     
        bro = bro->nextsibling->nextsibling;
        Stmt_t(bro, fType);
        bro = bro->nextsibling;
    }
}

void Def_t(struct Node *node)
{
    if (node == NULL || strcmp(node->type, "Def") != 0){
        printf("The node is not a Def.\n");
        return;
    }
    struct Node *child = node->arity;
    if (strcmp(child->type, "Specifier") == 0){
        enum KType LocalVarType = Specifier_t(child);
        if (LocalVarType == ERROR){
            printf("LType Error.\n");
            return;
        }
        struct Node *bro = child->nextsibling;
        DecList_t(bro, LocalVarType);
    }
}

void DecList_t(struct Node *node, enum KType LocalVarType)
{
    if (node == NULL || strcmp(node->type, "DecList") != 0){
        printf("The node is not a DecList.\n");
        return;
    }
    struct Node *child = node->arity;
    Dec_t(child, LocalVarType);
    if (child->nextsibling != NULL){
        DecList_t(child->nextsibling->nextsibling, LocalVarType);
    }
}

void Dec_t(struct Node *node, enum KType LocalVarType)
{
    if (node == NULL || strcmp(node->type, "Dec") != 0){
        printf("The node is not a Dec.\n");
        return;
    }
    struct Node *child = node->arity;
    struct SymbolMsg *mes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    struct FuncMsg *mes2 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
    struct ArrMsg *tmp = NULL;
    VarDec_t(child, LocalVarType, tmp);
    enum KType eType;
    if (child->nextsibling != NULL){
        Exp_t(child->nextsibling->nextsibling, mes1, mes2);
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

void StructSpecifier_t(struct Node *node){

}

void Exp_t(struct Node *node, struct SymbolMsg *SMes, struct FuncMsg *FMes)
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
    int f;
    //if not defined 
    FMes->rtType = 4;
    SMes->symType = 4;
    if (strcmp(child->type, "LP") == 0){

    }
    if (strcmp(child->type, "MINUS") == 0){
        
    }
    if (strcmp(child->type, "NOT") == 0){

    }
    if (strcmp(child->type, "ID") == 0)
    {
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
                    switch (tmp->symType){
                        case 0: SMes->symType = INT; break;
                        case 1: SMes->symType = FLOAT; break;
                    }
                    SMes->arr = tmp->arr;
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
            f = inSymList(child->text, &xType);
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
                        switch (tmp->rtType){
                            case 0: FMes->rtType = INT; break;
                            case 1: FMes->rtType = FLOAT; break;
                        }
                        //printf("%d in exp id func\n", FMes->rtType);
                        break;
                }
                tmp = tmp->nextFunc;
                }
                if (!f) {
                    printf("Error Type 2 at Line %d: Undefined function '%s'.\n", child->line, child->text);  
                    return;
                }
                if (strcmp(child->nextsibling->nextsibling->type, "Args") == 0){
                    struct ArgList *args = (struct ArgList *)malloc(sizeof(struct ArgList));
                    args = Args_t(child->nextsibling->nextsibling); 
                    int num = 0;
                    struct ArgList *pp = args;
                    while (pp != NULL)
                    {
                        num++;
                        pp = pp->nextarg;
                    }
                    //printf("%din exp id arg\n", num);
                    
                }   
            }
        }
    }
    else if (strcmp(child->type, "INT") == 0)
    {
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
            //printf("in exp exp\n"); 
            f = 1;
            if (strcmp(child->arity->type, "ID") == 0) f = 0;
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
            Exp_t(child, smes1, fmes1);
            Exp_t(bro->nextsibling, smes2, fmes2);
            if (smes1->symType != smes2->symType) printf("Error Type 5 at Line %d: Type mismatched for assignment.\n", child->line);
        }
        else if (strcmp(bro->type, "LB") == 0)
        {
            //struct SymbolMsg *smes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
            //struct FuncMsg *fmes1 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
            Exp_t(child, smes1, fmes1);
            if (smes1->arr == NULL) printf("Error Type 10 at Line %d: '%s' is not an array.\n", child->line, smes1->name);
            //struct SymbolMsg *smes2 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
            //struct FuncMsg *fmes2 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
            Exp_t(child, smes1, fmes1);
            SMes->symType = smes1->symType;
            //struct SymbolMsg *smes3 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
            //struct FuncMsg *fmes3 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
            Exp_t(child->nextsibling->nextsibling, smes1, fmes1);
            if (smes1->symType != 0) printf("Error Type 12 at Line %d: '%s' is not an integer\n", child->line, smes1->name);
        }
        else if (strcmp(bro->type, "DOT") == 0){

            }
        else{    //op:and or relop plus minus star div 
            //struct SymbolMsg *smes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
            //struct FuncMsg *fmes1 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
            //struct SymbolMsg *smes2 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
            //struct FuncMsg *fmes2 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
            //printf("%d!!!!\n", smes1->symType);
            f = 0;
            enum KType e1, e2;
            Exp_t(child, smes1, fmes1);
            Exp_t(child->nextsibling->nextsibling, smes2, fmes2);
            //************
            if (smes1->symType == 4 && fmes1->rtType != 4) e1 = fmes1->rtType;
            if (smes1->symType != 4 && fmes1->rtType == 4) e1 = smes1->symType;
            if (smes2->symType == 4 && fmes2->rtType != 4) e2 = fmes2->rtType;
            if (smes2->symType != 4 && fmes2->rtType == 4) e2 = smes2->symType;
            //printf("%d  %d\n", e1, e2);
            if (e1 != e2) printf("Error Type 7 at Line %d: Type mismatched for operands.\n", child->line);
        }
    }
}

struct ArgList *Args_t(struct Node *node)
{
    if (node == NULL || strcmp(node->type, "Args") != 0){
        printf("The node is not a Args.\n");
        return NULL;
    }
    //printf("in args\n");
    struct SymbolMsg *smes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    struct FuncMsg *fmes1 = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));
    struct Node *child = node->arity;
    struct ArgList *p = (struct ArgList *)malloc(sizeof(struct ArgList));
    Exp_t(child, smes1, fmes1);
    //**********
    if (smes1->symType == 4 && fmes1->rtType != 4) p->argType = fmes1->rtType;
    if (smes1->symType != 4 && fmes1->rtType == 4) p->argType = smes1->symType;
    p->nextarg = NULL;
    if (child->nextsibling != NULL){
        p->nextarg = Args_t(child->nextsibling->nextsibling);
    }
    return p;
}

void newSymbol(struct Node *node, enum KType sType, struct ArrMsg *vArray)
{
    //printf("in newsymbol\n");
    unsigned int hashval = hash_pjw(node->text);
    struct SymbolMsg *tmp = symbolTable[hashval];
    while (tmp != NULL){
        if (strcmp(tmp->name, node->text) == 0) 
            printf("Error Type 3 at Line %d: Redefined variable '%s'.\n", node->line, node->text);
        tmp = tmp->nextSym;
    }
    struct SymbolMsg *newsym = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));   //?
    strcpy(newsym->name, node->text);
    newsym->lineNumber = node->line;
    if (sType == 0) newsym->symType = 0;
    else if (sType == 1) newsym->symType = 1;
    newsym->nextSym = NULL;
    if (vArray != NULL){
        newsym->arr = vArray;
    }
    else newsym->arr = NULL;

    if (symbolTable[hashval] = NULL) 
        symbolTable[hashval] = newsym;
    else{
        newsym->nextSym = symbolTable[hashval];
        symbolTable[hashval] = newsym;
    }
}

void newFunc(struct Node *node, struct ArgList *args, enum KType fType)
{
    unsigned int fhashval = hash_pjw(node->text);
    struct FuncMsg *tmp = funcTable[fhashval];
    //printf("in newfunc.\n");
    while (tmp != NULL){
        if (strcmp(tmp->name, node->text) == 0)
            printf("Error Type 4 at Line %d: Redefined function '%s'.\n", node->line, node->text);
        tmp = tmp->nextFunc;
    }
    struct FuncMsg *newfunc = (struct FuncMsg *)malloc(sizeof(struct FuncMsg));   //?
    strcpy(newfunc->name, node->text);
    newfunc->rtType = fType;
    newfunc->lineNumber = node->line;
    struct ArgList *p = args;
    int num = 0;
    while (p != NULL)
        {
            num++;
            p = p->nextarg;
        }
    newfunc->argnum = num;
    newfunc->Args = args;
    newfunc->nextFunc = NULL;
    if (funcTable[fhashval] = NULL)
        funcTable[fhashval] = newfunc;
    else{
        newfunc->nextFunc = funcTable[fhashval];
        funcTable[fhashval] = newfunc;
    } 
}

int inSymList(char *text, enum KType *xType)
{
    unsigned int hashval = hash_pjw(text);
    struct SymbolMsg *tmp = symbolTable[hashval];
    int f = 0;
    while (tmp != NULL)
    {
        if (strcmp(tmp->name, text) == 0){
            f = 1;
            *xType = tmp->symType;
            break;
        }
        tmp = tmp->nextSym;
    }
    if (f) return 1; 
    else return 0;
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

