/*************************************************************************
	> File Name: xxx.c
	> Author: 
	> Mail: 
	> Created Time: Wed 30 Mar 2016 06:42:17 PM CST
 ************************************************************************/

#include<stdio.h>
extern FILE* yyin;
int main(int argc, char** argv){
    if (argc>1){
        if (!yyin = fopen(argv[1], "r")){
            perror(argv[1]);
            return 1;
        }
    }
    while (yylex()!= 0);
    return 0;
}
