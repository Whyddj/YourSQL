#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "syntax.tab.h"
#include <iostream>

int yylex(void);
FILE* yyin;

int main(int argc, char **argv)
{
	
	yyparse();
	
	//printf("before show  &  after yyparse\n");
	
	//printf("after show\n");

	return 0;
}