#include<stdio.h>
#include<string.h>
#include"semantic.h"
#include"intercode.h"

extern int yylineno;
extern struct Node *root;
extern int err;

int main(int argc, char** argv)

{
	if (argc <= 1) return 1;
	    
        err = 0;
        yylineno = 1;
        FILE* f = fopen(argv[1], "r");
	    if (!f)
	    {
		    perror(argv[1]);
		    return 1;
	    }
	    yyrestart(f);
	    yyparse();
        if (root != NULL && err == 0) PrintTree(root, 0);
        semantic();
        //******
        if (argc == 3) {
            traverse_gen_ir(argv[2]);
        }
        printf("\n\n");
	
    return 0;
}

