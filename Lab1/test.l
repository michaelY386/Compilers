%option yylineno

%{
	#include<stdio.h>
    #include<stdlib.h>
	#include<string.h> 
	#include"syntax.tab.h"
   
    extern err;
    struct Node{
        int terminal;
        int line;
        char text[30];
        char type[30];
        struct Node *arity;
        struct Node *nextsibling;
    };
    void CreateNode(char *text, char *type);
%}    
DELIM [ \t\n]
WS {DELIM}+
DIGIT [0-9]
LETTER [_a-zA-Z]
INT [+-]?[0]|[1-9][0-9]*	
INT8 [0][1-7][0-7]*
INT16 [0][xX][1-9a-fA-F][0-9a-fA-F]*
FLOAT [+-]?([0-9]*(\.[0-9]*)?)([eE][+-]?([0-9]*)?)
ID ({LETTER})({LETTER}|{DIGIT})*
RELOP >|<|>=|<=|==|!=
TYPE int|float

%%
{WS}    {}
{TYPE}	{CreateNode(yytext, "TYPE"); return TYPE;}
{RELOP}	{CreateNode(yytext, "RELOP"); return RELOP;}
{FLOAT}	{CreateNode(yytext, "FLOAT"); return FLOAT;}
{INT}	{CreateNode(yytext, "INT"); return INT;}
{INT8}  {CreateNode(yytext, "INT"); return INT;}
{INT16} {CreateNode(yytext, "INT"); return INT;}
if		{CreateNode(yytext, "IF"); return IF;}
else	{CreateNode(yytext, "ELSE"); return ELSE;}
while	{CreateNode(yytext, "WHILE"); return WHILE;}
return	{CreateNode(yytext, "RETURN"); return RETURN;}
struct	{CreateNode(yytext, "STRUCT"); return STRUCT;}
{ID}	{CreateNode(yytext, "ID"); return ID;} 
"="		{CreateNode(yytext, "ASSIGNOP"); return ASSIGNOP;}
";"		{CreateNode(yytext, "SEMI"); return SEMI;}
","		{CreateNode(yytext, "COMMA"); return COMMA;}
"+"		{CreateNode(yytext, "PLUS"); return PLUS;}
"-"		{CreateNode(yytext, "MINUS"); return MINUS;}
"//"    {char c = input(); while (c != '\n') c = input();}
"/*"    {char c = input(); 
         while (c!=EOF)
            {
                if (c == '*') 
                    {
                        char cc = input();
                        if (cc == '/') break;
                            else c =cc;
                    }
                else c = input();
            }
        }
"/"		{CreateNode(yytext, "DIV"); return DIV;}
"*"		{CreateNode(yytext, "STAR"); return STAR;}
"||"	{CreateNode(yytext, "OR"); return OR;}
"&&"	{CreateNode(yytext, "AND"); return AND;}
"("		{CreateNode(yytext, "LP"); return LP;}
")"		{CreateNode(yytext, "RP"); return RP;}
"["		{CreateNode(yytext, "LB"); return LB;}
"]"		{CreateNode(yytext, "RB"); return RB;}
"{"		{CreateNode(yytext, "LC"); return LC;}
"}"		{CreateNode(yytext, "RC"); return RC;}
"."		{CreateNode(yytext, "DOT"); return DOT;}
"!"		{CreateNode(yytext, "NOT"); return NOT;}
.		{err = 1; printf("Error type A at Line %d:Mysterious character \'%s\'\n", yylineno, yytext);}
%%

void CreateNode(char *text, char *type)
{
    yylval.node = (struct Node *)malloc(sizeof(struct Node));
    yylval.node->terminal = 1;
    yylval.node->line = yylineno;
    yylval.node->arity = NULL;
    yylval.node->nextsibling = NULL;
    strcpy(yylval.node->type, type);
    strcpy(yylval.node->text, text);
}

