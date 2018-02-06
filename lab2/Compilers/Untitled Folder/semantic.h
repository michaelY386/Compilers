/*************************************************************************
	> File Name: semantic.h
	> Author: 
	> Mail: 
	> Created Time: Fri 22 Apr 2016 04:46:29 PM CST
 ************************************************************************/
#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include"node.h"
#define tablesize 30000

enum KType{
    INT, FLOAT, ARRAY, STRUCT, ERROR
}; 

struct ArrMsg{
    int size;
    enum KType aType;
    struct ArrMsg *nextarrType;
};

struct SymbolMsg{
    char name[30];
    int visitedTag;
    int lineNumber;
    enum KType symType;
    struct SymbolMsg *nextSym;
    struct ArrMsg *arr;
};

struct FuncMsg{
    char name[30];
    int visitedTag;
    int argnum;
    int lineNumber;
    enum KType rtType;
    struct ArgList *Args;
    struct FuncMsg *nextFunc;
};


struct ArgList{
    enum KType argType;
    char name[30];
    struct ArgList *nextarg;
};

void semantic();
void init();
void Program_t(struct Node *node);
void ExtDefList_t(struct Node * node);
void ExtDef_t(struct Node *node);
enum KType Specifier_t(struct Node *node);
void ExtDecList_t(struct Node *node, enum KType sType);
void VarDec_t(struct Node *node, enum KType sType, struct ArrMsg *vArray);
void FunDec_t(struct Node *node, enum KType sType);
struct ArgList *VarList_t(struct Node *node);
struct ArgList *ParaDec_t(struct Node *node);
void CompSt_t(struct Node *node, enum KType fType);
void DefList_t(struct Node *node);
void StmtList_t(struct Node *node, enum KType fType);
void Stmt_t(struct Node *node, enum KType fType);
void Def_t(struct Node *node);
void DecList_t(struct Node *node, enum KType LocalVarType);
void Dec_t(struct Node *node, enum KType LocalVarType);
unsigned int hash_pjw(char* name);
void StructSpecifier_t(struct Node *node);
void Exp_t(struct Node *node, struct SymbolMsg *SMes, struct FuncMsg *FMes);
struct ArgList *Args_t(struct Node *node);

void newSymbol(struct Node *, enum KType sType, struct ArrMsg *vArray);
void newFunc(struct Node *node, struct ArgList *args, enum KType fType);

int inSymList(char *text, enum KType *xType);

#endif
