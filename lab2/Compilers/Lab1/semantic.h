/*************************************************************************
	> File Name: semantic.h
	> Author: 
	> Mail: 
	> Created Time: Fri 22 Apr 2016 04:46:29 PM CST
 ************************************************************************/
#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include"lexical.h"
#define tablesize 3000

struct SymbolMsg{
    char name[30];
    int visitedTag;
    int lineNumber;
    char symType[30];
    struct SymbolMsg *nextSym;
};

struct FuncMsg{
    char name[30];
    int visitedTag;
    int lineNumber;
    char funcType[30];
    struct FuncMsg *nextFunc;
};

struct SymbolMsg *symbolTable[tablesize];
struct FuncMsg *funcTable[tablesize];

enum KType{
    INT, FLOAT, ARRAY, STRUCT, ERROR
};

struct VarMsg{
    char name[30];
    enum KType varType;
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
void VarDec_t(struct Node *node, enum KType sType);
void FunDec_t(struct Node *node, enum KType sType);
struct ArgList VarList_t(struct Node *node);
void ParaDec_t(struct Node *node, enum KType *argType, char **name);
void CompSt_t(struct Node *node);
void DefList_t(struct Node *node);
void StmtList_t(struct Node *node);
void Stmt_t(struct Node *node);
void Def_t(struct Node *node);
void DecList_t(struct Node *node, enum KType LocalVarType);
void Dec_t(struct Node *node, enum KType LocalVarType);
unsigned int hash_pjw(char* name);


void newSymbol();

#endif
