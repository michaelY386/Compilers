%{
	#include"lex.yy.c"
	#include<stdlib.h>
	#include<string.h>
	#include<stdio.h>
    #include<stdarg.h>

    int err;

    struct Node *root;
    struct Node *addNode(char *type, int num, ...);
    void PrintTree(struct Node *a, int dep);
%}

%union{struct Node *node;}

%token <node> INT FLOAT SEMI COMMA ID TYPE
%right <node> ASSIGNOP
%left <node> OR
%left <node> AND
%left <node> RELOP
%left <node> PLUS MINUS 
%left <node> STAR DIV
%right <node> NOT
%left <node> DOT LB RB LP RP LC RC

%nonassoc <node> LOWER_THAN_ELSE
%nonassoc <node> IF ELSE WHILE STRUCT RETURN
%nonassoc <node> UMINUS

%type <node> Program ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier OptTag Tag
%type <node> VarDec FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt
%type <node> DefList Def DecList Dec
%type <node> Exp Args
%error-verbose

%%
Program	:	ExtDefList {$$ = addNode("Program", 1, $1);root = $$;}
		;
ExtDefList	 :	ExtDef ExtDefList {$$ = addNode("ExtDefList", 2, $1, $2);}
		    | {$$ = NULL;}
			;
ExtDef	:	Specifier ExtDecList SEMI {$$ =addNode("ExtDef", 3, $1, $2, $3);}
	    |	Specifier SEMI {$$ = addNode("ExtDef", 2, $1, $2);}
		|	Specifier FunDec CompSt{$$ = addNode("ExtDef", 3, $1, $2, $3);} 
        |   Specifier FunDec SEMI {$$ = addNode("ExtDef", 3, $1, $2, $3);}
        |   Specifier error {err = 1;}
        |   Specifier error SEMI {err = 1;}
        |   Specifier ExtDecList error {err = 1;}
        |   error SEMI {err = 1;}
        ;
ExtDecList	:	VarDec {$$ = addNode("ExtDecList", 1, $1);}
			|	VarDec COMMA ExtDecList {$$ = addNode("ExtDecList", 3, $1, $2, $3);}
			;
Specifier	:	TYPE {$$ = addNode("Specifier", 1, $1);}
        	|	StructSpecifier {$$ = addNode("Specifier", 1, $1);}
			;
StructSpecifier	:	STRUCT OptTag LC DefList RC {$$ = addNode("StructSpecifier",5, $1, $2, $3, $4, $5);}
				|	STRUCT Tag {$$ = addNode("StructSpecifier", 2, $1, $2);}
			    |   STRUCT error RC {err = 1;}
                ;
OptTag	:	ID {$$ = addNode("OptTag", 1, $1);}
		|	{$$ = NULL;}
		;
Tag		:	ID {$$ = addNode("Tag", 1, $1);}
		;
VarDec	:	ID {$$ = addNode("VarDec", 1, $1);}
		|	VarDec LB INT RB {$$ =addNode("VarDec", 4, $1, $2, $3, $4);}
        |   error RB {err = 1;}
        ;
FunDec	:	ID LP VarList RP {$$ = addNode("FunDec", 4, $1, $2, $3, $4);}
		|	ID LP RP {$$ = addNode("FunDec", 3, $1, $2, $3);}
        |   ID LP error RP {err = 1;}
        ;
VarList	:	ParamDec COMMA VarList {$$ = addNode("VarList", 3, $1, $2, $3);}
		|	ParamDec {$$ = addNode("VarList", 1, $1);}
        ;
ParamDec	:	Specifier VarDec {$$ = addNode("ParamDec", 2, $1, $2);}
            ;
CompSt	:	LC DefList StmtList	RC {$$ = addNode("CompSt", 4, $1, $2, $3, $4);}
        |   error RC {err = 1;}
        ;
StmtList	:	Stmt StmtList {$$ = addNode("StmtList", 2, $1, $2);}
			|	{$$ = NULL;}
			;
Stmt		:	Exp SEMI {$$ = addNode("Stmt", 2, $1, $2);}
			|	CompSt {$$ = addNode("Stmt", 1, $1);}
			|	RETURN Exp SEMI {$$ = addNode("Stmt", 3, $1, $2, $3);}
			|	IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = addNode("Stmt", 5, $1, $2, $3, $4, $5);}
            |   IF LP Exp RP Stmt ELSE Stmt {$$ = addNode("Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
			|	WHILE LP Exp RP Stmt {$$ = addNode("Stmt", 5, $1, $2, $3, $4, $5);}
            |   error SEMI {err = 1;}
            |   Exp error {err = 1;}
            ;
DefList		:	Def DefList {$$ = addNode("DefList", 2, $1, $2);}
			|   {$$ = NULL;}
			;
Def			:	Specifier DecList SEMI {$$ = addNode("Def", 3, $1, $2, $3);}
            |   Specifier FunDec SEMI {$$ = addNode("Def", 3, $1, $2, $3);}
            |   Specifier DecList error {err = 1;}
            ;
DecList		:	Dec {$$ = addNode("DecList", 1, $1);}
			|	Dec COMMA DecList {$$ = addNode("DecList", 3, $1, $2, $3);}
			;
Dec			:	VarDec {$$ = addNode("Dec", 1, $1);}
			|	VarDec ASSIGNOP Exp {$$ = addNode("Dec", 3, $1, $2, $3);}
			;
Exp			:	Exp ASSIGNOP Exp {$$ = addNode("Exp", 3, $1, $2, $3);}
			|	Exp AND Exp {$$ = addNode("Exp", 3, $1, $2, $3);}
			|	Exp OR Exp {$$ = addNode("Exp", 3, $1, $2, $3);}
			|	Exp RELOP Exp {$$ = addNode("Exp", 3, $1, $2, $3);}
			|	Exp PLUS Exp {$$ = addNode("Exp", 3, $1, $2, $3);}
			|	Exp MINUS Exp {$$ = addNode("Exp", 3, $1, $2, $3);}
			|	Exp STAR Exp {$$ = addNode("Exp", 3, $1, $2, $3);}
			|	Exp DIV Exp {$$ = addNode("Exp", 3, $1, $2, $3);}
			|	LP Exp RP {$$ = addNode("Exp", 3, $1, $2, $3);}
            |   MINUS Exp %prec UMINUS {$$ = addNode("Exp", 2, $1, $2);}
	        |   NOT Exp {$$ = addNode("Exp", 2, $1, $2);}
			|	ID LP Args RP {$$ = addNode("Exp", 4, $1, $2, $3, $4);}
			|	ID LP RP {$$ = addNode("Exp", 3, $1, $2, $3);}
			|	Exp LB Exp RB {$$ = addNode("Exp", 4, $1, $2, $3, $4);}
			|	Exp DOT	ID {$$ = addNode("Exp", 3, $1, $2, $3);}
			|	ID {$$ = addNode("Exp", 1, $1);}
			|	INT {$$ = addNode("Exp", 1, $1);}
			|	FLOAT {$$ = addNode("Exp", 1, $1);}
            |   error RP {err = 1;}
            |   Exp LB error RB {err = 1;}
            ;
Args		:	Exp COMMA Args {$$ = addNode("Args", 3, $1, $2, $3);}
			|	Exp {$$ = addNode("Args", 1, $1);}
 			;
%%

struct Node *addNode(char* type, int arg_num, ...)
{
    int i;
    va_list vl;
    va_start(vl, arg_num);
    struct Node *p = (struct Node *)malloc(sizeof(struct Node));
    struct Node *arity = (struct Node *)malloc(sizeof(struct Node));
    strcpy(p->type, type);
    p->terminal = 0;
    arity = va_arg(vl, struct Node*);
    p->line = arity->line;
    p->arity = arity;
    for (i = 1; i < arg_num; i++)
    {
        arity->nextsibling  = va_arg(vl, struct Node*);
        if (arity->nextsibling != NULL) 
            {
                arity = arity->nextsibling;
            }
    }
    arity->nextsibling = NULL;
    va_end(vl);
    return p;
}


void PrintTree(struct Node *a, int dep)
{
    int d;
    int val;
    if (a ==  NULL) return;
    for (d = 0; d < dep; d++) printf("  ");
    if (a->terminal == 0) 
        {
            printf("%s (%d)\n", a->type, a->line);
            PrintTree(a->arity, dep + 1);
        }
    else 
        {
            if (strcmp(a->type, "INT") == 0)
                {
                  if (a->text[0] == '0')
                        if (a->text[1] == 'x' || a->text[1] == 'X')
                            val = (int)strtoul(a->text+2,0,16);
                        else val = (int)strtoul(a->text+1,0,8);
                  else val = atoi(a->text);
                  printf("INT: %d\n", val);
                }
            else if (strcmp(a->type, "TYPE") == 0)
                printf("TYPE: %s\n", a->text);
            else if (strcmp(a->type, "ID") == 0)
                printf("ID: %s\n", a->text);
            else if (strcmp(a->type, "FLOAT") == 0)
                printf("FLOAT: %f\n", atof(a->text));
            else printf("%s\n", a->type);
        }
    PrintTree(a->nextsibling, dep);
}

int yyerror(char* msg)
{
    fprintf(stderr, "Error type B at Line %d:%s\n", yylineno, msg);
    return 0;
}
