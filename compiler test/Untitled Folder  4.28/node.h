/*************************************************************************
	> File Name: lexical.h
	> Author: 
	> Mail: 
	> Created Time: Fri 22 Apr 2016 04:48:11 PM CST
 ************************************************************************/
#ifndef _LEXICAL_H
#define _LEXICAL_H
struct Node{
    int terminal;
    int line;
    char text[30];
    char type[30];
    struct Node *arity;
    struct Node *nextsibling;
};
#endif
