/*************************************************************************
	> File Name: semantic.c
	> Author: 
	> Mail: 
	> Created Time: Fri 22 Apr 2016 04:30:57 PM CST
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"lexical.h"
#include"semantic.h"
#define tablesize 3000


void semantic()
{
    init();
    Program_t(root);
}

void init()
{
    memset(symbolTable, 0, sizeof(SymbolMsg *)*tablesize);
    memset(funcTable, 0, sizeof(FuncMsg *)*tablesize);
}

/* Traversing the tree */
void Program_t(struct Node *node)
{
    if (node == NULL || strcmp(node->type, "Program") != 0) 
    {
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
                FunDec_t(bro);
                Compst_t(bro);
            }
            else if (strcmp(bro->type), "SEMI") == 0){
                // do nothing
            }
    }
}

enum KType Specifier_t(struct Node *node)
{
    if (node == NULL || strcmp(node->type, "Specifier") != 0)
    {
        printf("The node is not a Specifier.\n");
        return NULL;
    }
    struct Node *child = node->arity;
    if (child != NULL){
        if (strcmp(child->type, "TYPE") == 0)
            if (strcmp(child->type->text, "int") == 0)
                return INT;
            else if (strcmp(child->type->text, "float") == 0)
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
        VarDec_t(child, sType);
        struct Node *bro = child->nextsibling;
        if (bro !== NULL) 
            ExtDecList_t(child->nextsibling->nextsibling);
        }
}
        
struct VarMsg VarDec_t(struct Node *node, enum KType sType)
{
    if (node == NULL || strcmp(node->type, "VarDec") != 0){
        printf("The node is not a VarDec.\n");
        return NULL;
    }
    struct Node *child = node->arity;
    if (child != NULL){
        if (strcmp(child->type, "ID") == 0){
            newSymbol(child, sType);
        }
        else (strcmp(child->type, "VarDec") == 0){
            VarDec_t(child, sType);
        }
    }
}

void FunDec_t(struct Node *node, enum KType sType)
{
    
    if (node == NULL || strcmp(node->type, "FunDec") != 0){
        printf("The node is not a FunDec.\n");
        return;
    }
    struct Node *child = node->arity;
    struct Arglist * args;
    if (strcmp(child->nextsibling->nextsibling, "VarList") == 0){
        args = VarList_t(child->nextsibling->nextsibling);
    }
    else args = NULL;    // the function does not have any parameters
    newFunc(child, args, sType);    
}

struct Arglist *VarList_t(struct Node *node)
{
    
    if (node == NULL || strcmp(node->type, "VarList") != 0){
        printf("The node is not a VarList.\n");
        return NULL;
    }
    struct Node *child = node->arity;
    struct Arglist *p = malloc(sizeof(struct Arglist));   //?
    ParaDec_t(child, &(p->argType), &(p->name));    
    p->next = NULL;    //?
    if (child->nextsibling != NULL){
        p->next = VarList_t(child->nextsibling->nextsibling);
    }
    return p;
}

void ParaDec_t(struct Node *node, enum KType *argType, char **name)
{
     
    if (node == NULL || strcmp(node->type, "ParaDec") != 0){
        printf("The node is not a ParaDec.\n");
        return;
    }
    struct Node *child = node->arity;
    *argType = Specifier_t(child);
    if (argType == ERROR) return;
    struct VarMsg *var = VarDec_t(child->nextsibling, argType);
    *name = var->name; 
}

void CompSt_t(struct Node *node)
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
            StmtList_t(bro);
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
            DefList(child->nextsibling);
        }
    }
}
 
void StmtList_t(struct Node *node)
{
    if (node == NULL || strcmp(node->type, "StmtList") != 0){
        printf("The node is not a StmtList.\n");
        return;
    }
    struct Node *child = node->arity;
    if (child != NULL){
        Stmt_t(child);
        if (child->nextsibling != NULL){
            StmtList_t(child->nextsibling);
        }
    }
}

void Stmt_t(struct Node *node)
{
    if (node == NULL || strcmp(node->type, "Stmt") != 0){
        printf("The node is not a Stmt.\n");
        return;
    }
    struct Node *child = node->arity;
    if (strcmp(child->type, "Exp") == 0){
        Exp_t(child);                         //*
    }
    else if (strcmp(child->type, "CompSt") == 0){
        Compst_t(child);                     //*
    }
    else if (strcmp(child->type, "RETURN") == 0){
        Exp_t(child->nextsibling);           //*
    }
    else if (strcmp(child->type, "IF") == 0){
        struct Node *bro = child->nextsibling->nextsibling;
        Exp_t(bro);                         //*
        bro = bro->nextsibling->nextsibling;
        Stmt_t(bro);
        bro = bro->nextsibling;
        if (bro != NULL && strcmp(bro->type, "ELSE")==0){
            bro = bro->nextsibling;
            Stmt_t(bro);
        }
    }
    else if (strcmp(child->type, "WHILE") == 0){
        struct Node *bro = child->nextsibling->nextsibling;
        Exp_t(bro);                         //*
        bro = bro->nextsibling->nextsibling;
        Stmt_t(bro);
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
        DecList_t(child->nextsibling->nextsibling);
    }
}


void Dec_t(struct Node *node, enum KType LocalVarType)
{
    if (node == NULL || strcmp(node->type, "DecList") != 0){
        printf("The node is not a DecList.\n");
        return;
    }
    struct Node *child = node->arity;
    VarDec_t(child, LocalVarType);
    if (child->nextsibling != NULL){
        Exp_t(child->nextsibling->nextsibling);
    }
}

unsigned int hash_pjw(char* name)
{
    unsigned int val = 0;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff) 
            val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

void newSymbol(struct Node *node, enum KType sType)
{
    unsigned int hashval = hash_pjw(node->text);
    struct SymbolMsg *tmp = symbolTable[hashval];
    while (tmp != NULL){
        if (strcmp(tmp->name, node->text) == 0) printf("Error 3.\n");
        tmp = tmp->nextSym;
    }
    struct SymbolMsg *newsym = (struct SymbolMsg *)malloc(sizeof(struct SymbolMsg));   //?
    newsym->name = node->text;
    newsym->linenumber = node->line;
    strcpy(newsym->symType, node->type);
    newsym->nextSym = NULL;

    if (symbolTable[hashval] = NULL) 
        symbolTable[hashval] = newsym;
    else{
        newsym->next = symbolTable[hashval];
        symbolTable[hashval] = newsym;
    }
}














