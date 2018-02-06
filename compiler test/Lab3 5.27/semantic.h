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
    //********
    char strtype[30];  // var in which struct
    int var_no;        // for lab3
    int visitedTag;
    int lineNumber;
    int isArray;   // if it is int array[], then isArray = 1, symType = 0;
    int dimension;  // array dimension
    int isStr;     // when time period in struct deflist
    int inStr;     // = 1 if the var is in a struct
    enum KType symType;
    struct SymbolMsg *nextSym;
    struct ArrMsg *arr;            // not null if isArray = 1
    struct FieldMsg *fieldList;    // not null if is Str = 1
    int err;   // = 1 if the struct def is wrong 
};

struct FuncMsg{
    char name[30];
    char strtype[30];
    int visitedTag;
    int argnum;
    int isArray;  // ===0
    int dimension;
    int isDef;   //  = 1 if it is defined
    int lineNumber;
    enum KType rtType;
    struct ArgList *Args;
    struct FuncMsg *nextFunc;
};
/*
struct StrMsg
{
    char name[30];
    int visitedTag;
    int isStr;
    int lineNumber;
    struct FieldMsg *fieldList;
    struct StrMsg *nextStr;
};
*/
struct FieldMsg
{
    char name[30];
    enum KType fieldType;
    struct FieldMsg *nextField; 
};

struct ArgList{
    enum KType argType;
    char name[30];
    struct ArgList *nextarg;
};

void semantic();
void read_write();
void init();
void Program_t(struct Node *node);
void ExtDefList_t(struct Node * node);
void ExtDef_t(struct Node *node, struct SymbolMsg *newstr);
enum KType Specifier_t(struct Node *node, struct SymbolMsg *newstr);
void ExtDecList_t(struct Node *node, enum KType sType, struct SymbolMsg *newstr);
void StructSpecifier_t(struct Node *node, struct SymbolMsg *newstr);
void OptTag_t(struct Node *node, struct SymbolMsg *newstr);
void Tag_t(struct Node *node, struct SymbolMsg *newstr);
void VarDec_t(struct Node *node, enum KType sType, struct ArrMsg *vArray,struct SymbolMsg *newstr);
void FunDec_t(struct Node *node, enum KType sType, struct SymbolMsg *newstr, int defed);
struct ArgList *VarList_t(struct Node *node, struct SymbolMsg *newstr);
struct ArgList *ParaDec_t(struct Node *node, struct SymbolMsg *newstr);
void CompSt_t(struct Node *node, enum KType fType, struct SymbolMsg *newstr);
void DefList_t(struct Node *node, struct SymbolMsg *newstr);
void StmtList_t(struct Node *node, enum KType fType, struct SymbolMsg *newstr);
void Stmt_t(struct Node *node, enum KType fType, struct SymbolMsg *newstr);
void Def_t(struct Node *node, struct SymbolMsg *newstr);
void DecList_t(struct Node *node, enum KType LocalVarType, struct SymbolMsg *newstr);
void Dec_t(struct Node *node, enum KType LocalVarType, struct SymbolMsg *newstr);
unsigned int hash_pjw(char* name);
void Exp_t(struct Node *node, struct SymbolMsg *SMes, struct FuncMsg *FMes, struct SymbolMsg *newstr);
struct ArgList *Args_t(struct Node *node, struct SymbolMsg *newstr);

void newSymbol(struct Node *, enum KType sType, struct ArrMsg *vArrayi, struct SymbolMsg *newstr);
void newFunc(struct Node *node, struct ArgList *args, enum KType fType, int defed);

int inSymList(char *text, enum KType *xType, int *k);
int lookup_stable(char *text, enum KType *x);
struct FuncMsg *lookup_ftable(char *text);
struct SymbolMsg * getVar(char *text);
#endif
