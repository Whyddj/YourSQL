#include "stdio.h"
#include "string.h"
#include "stdlib.h"

extern FILE* yyin;
extern int yyparse();
extern int yylex();


int main(int argc, char **argv)
{
	int i=0;
	if(argc > 2 )
	{
		printf("argcs too many!.\n");
		return 0;
	}
	if(argc == 2){
		yyin = fopen(argv[1],"r");
	}else{
		printf("no file input!\n");
        	return 1;
	}

	
	yyparse();
	
	//printf("before show  &  after yyparse\n");
	
	//printf("after show\n");

	return 0;
}