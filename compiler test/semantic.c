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
                //printf("in vardec varray = null\n");
                struct ArrMsg *newarr = (struct ArrMsg *)malloc(sizeof(struct ArrMsg));
                newarr->aType = sType;
                newarr->size = atoi(child->nextsibling->nextsibling->text);
                newarr->nextarrType = NULL; 
                vArray = newarr;
                VarDec_t(child, sType, vArray);
            }
            else{
                //printf("!!!!!!!!!!!!\n");
                if (vArray == NULL) printf("1849327237\n");
                struct ArrMsg *tmp = vArray;
                //while (tmp->nextarrType != NULL)
                //    tmp = tmp->nextarrType;
                struct ArrMsg *newarr = (struct ArrMsg *)malloc(sizeof(struct ArrMsg));
                newarr->aType = 3;
                newarr->size = atoi(child->nextsibling->nextsibling->text);
                newarr->nextarrType = tmp;
                //if (vArray == NULL) printf("1849327237\n");
                //printf("in vardec%d  %d\n", newarr->size, newarr->aType);
                //tmp->nextarrType = newarr;
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
    //printf("in fundec.\n");
    struct Node *child = node->arity;
    struct ArgList *args = malloc(sizeof(struct ArgList));
    if (strcmp(child->nextsibling->nextsibling->type, "VarList") == 0){
        //printf("in fundec.\n");
        args = VarList_t(child->nextsibling->nextsibling);
        //printf("in fundec\n");
    }
    else args = NULL;    // the function does not have any parameters
    //printf("in fundec\n");
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
    //printf("in VarList.\n");
    //ParaDec_t(child);    
    strcpy(p->name, tmp->name);
    //printf("12345\n");
    p->argType = tmp->argType;
    //printf("%d\n", p->argType);
    if (child->nextsibling != NULL){
        //if (p->nextarg == NULL) printf("67891023124\n");
        p->nextarg = VarList_t(child->nextsibling->nextsibling);
    }
    //printf("11111\n");
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
    struct SymbolMsg *mes = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    enum KType eType;
    if (strcmp(child->type, "Exp") == 0){
        Exp_t(child, mes);                         //*
    }
    else if (strcmp(child->type, "CompSt") == 0){
        CompSt_t(child, fType);                     //*
    }
    else if (strcmp(child->type, "RETURN") == 0){
        Exp_t(child->nextsibling, mes);    //*
        //printf("%d   %d\n", fType, mes->symType);
        if (mes->symType != fType) printf("Error Type 8 at Line %d: Type mismatched for return.\n", child->line);
    }
    else if (strcmp(child->type, "IF") == 0){
        struct Node *bro = child->nextsibling->nextsibling;
        Exp_t(bro, mes);                         //*
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
        Exp_t(bro, mes);                         //*
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
    struct SymbolMsg *mes = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
    struct ArrMsg *tmp = NULL;
    VarDec_t(child, LocalVarType, tmp);
    enum KType eType;
    if (child->nextsibling != NULL){
        Exp_t(child->nextsibling->nextsibling, mes);
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

void Exp_t(struct Node *node, struct SymbolMsg *Mes)
{
    if (node == NULL || strcmp(node->type, "Exp") != 0){
        printf("The node is not a Exp.\n");
        return;
    }
    struct Node *child = node->arity;
    struct ArrMsg *ap;
    int f;
    //printf("in exp 1\n");
    if (strcmp(child->type, "LP") == 0){

    }
    if (strcmp(child->type, "MINUS") == 0){
        
    }
    if (strcmp(child->type, "NOT") == 0){

    }
    if (strcmp(child->type, "ID") == 0)
    {
        //printf("%s\n", child->text);
        if (child->nextsibling == NULL)    // ID alone
        {                                       
            unsigned int hashval = hash_pjw(child->text);
            struct SymbolMsg *tmp = symbolTable[hashval];
            f = 0;
            //printf("%s\n", child->text);
            while (tmp != NULL)
            {
                if (strcmp(tmp->name, child->text) == 0){
                    f = 1;
                    //printf("in exp id \n");
                    //printf("%s   %d!!!!!\n", tmp->name, tmp->symType);
                    switch (tmp->symType){
                        case 0: Mes->symType = INT; break;
                        case 1: Mes->symType = FLOAT; break;
                    }
                    //printf("%s\n", tmp->name);
                    Mes->arr = tmp->arr;
                    strcpy(Mes->name, tmp->name);
                    break;
                }
                tmp = tmp->nextSym;
            }
            if (!f) printf("Error Type 1 at Line %d: Undefined variable '%s'.\n", child->line, child->text);
        }
        else                        //function 
        {
            unsigned int fhashval = hash_pjw(child->text);
            struct FuncMsg *tmp = funcTable[fhashval];
            f = 0;
            while (tmp != NULL)
            {
                if (strcmp(tmp->name, child->text) == 0){
                    f = 1;
                    switch (tmp->rtType){
                        case 0: Mes->symType = INT; break;
                        case 1: Mes->symType = FLOAT; break;
                    }
                    break;
                }
                tmp = tmp->nextFunc;
            }
            if (!f) printf("Error Type 2 at Line %d: Undefined function '%s'.\n", child->line, child->text);
        }
    }
    if (strcmp(child->type, "INT") == 0)
    {
        Mes->symType = INT;
        strcpy(Mes->name, child->text);
    }
    if (strcmp(child->type, "FLOAT") == 0)
    {
        Mes->symType = FLOAT;
        strcpy(Mes->name, child->text);
    }
    if (strcmp(child->type, "Exp") == 0)
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
            struct SymbolMsg *mes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
            struct SymbolMsg *mes2 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));  
            Exp_t(child, mes1);
            Exp_t(bro->nextsibling, mes2);
            if (mes1->symType != mes2->symType) printf("Error Type 5 at Line %d: Type mismatched for assignment.\n", child->line);
        }
        else if (strcmp(bro->type, "LB") == 0)
        {
            struct SymbolMsg *newmes3 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
            Exp_t(child, newmes3);
            if (newmes3->arr == NULL) printf("Error Type 10 at Line %d: '%s' is not an array.\n", child->line, newmes3->name);
            struct SymbolMsg *newmes1 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
            Exp_t(child, newmes1);
            Mes->symType = newmes1->symType;
            struct SymbolMsg *newmes2 = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));
            Exp_t(child->nextsibling->nextsibling, newmes2);
            //printf("%d\n", eType);
            if (newmes2->symType != 0) printf("Error Type 12 at Line %d: '%s' is not an integer\n", child->line, newmes2->name);
        }
        else if (strcmp(bro->type, "DOT") == 0){

            }
        else{    //op:and or relop plus minus star div 

        }
    }
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
    //newsym->arr = NULL;
    if (vArray != NULL){
        //newsym->symType = 3;
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
    //printf("%d\n", fhashval);
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
    newfunc->Args = args;
    newfunc->nextFunc = NULL;
    if (funcTable[fhashval] = NULL)
        funcTable[fhashval] = newfunc;
    else{
        newfunc->nextFunc = funcTable[fhashval];
        funcTable[fhashval] = newfunc;
    } 
}










