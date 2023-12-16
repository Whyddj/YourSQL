%{
    #include "stdio.h"
    #include "stdlib.h"
    #include "string.h"
    #include "syntax.tab.h"
    union LEX_gather{
        int ivalue;
        int operator;
        char* Data;
        char* name;
    };
    union LEX_gather arr[1000000];
    extern int index=0;
%}

white [\n\t ]
ID [a-zA-Z_][a-zA-Z0-9_]*
DATA_STR [a-zA-Z_0-9\,\.\+-=;]+
INT [\+-]?[1-9][0-9]*


%%
{white} {}
{DATA_STR} {arr[index].Data = (char*)malloc(sizeof(yytext));strcpy(arr[index].Data,yytext);index++;printf("%s\n",yytext);}
{INT} {arr[index++].ivalue == atoi(yytext);return Y_INT;printf("%d\n",arr[index-1].ivalue);}
[\+-]?0 {arr[index++].ivalue = atoi(yytext);return Y_INT;printf("%d\n",arr[index-1].ivalue);}
{ID} {arr[index].name = (char*)malloc(sizeof(yytext));strcpy(arr[index].name,yytext);index++;printf("%s\n",yytext);}



"\"" {arr[index++].char = yytext[0];return Y_QUO;printf("%s\n",yytext);}
"(" {arr[index++].char = yytext[0];return Y_LPAR;printf("%s\n",yytext);}
")" {arr[index++].char = yytext[0];return Y_RPAR;printf("%s\n",yytext);}
";" {arr[index++].char = yytext[0];return Y_SEMICOLON;printf("%s\n",yytext);}
";"+.+\n {arr[index].Data = (char*)malloc(sizeof(yytext));strcpy(arr[index].Data,yytext);index++;printf("注释:%s\n",yytext);}




%%

void yyerror(char *str){
    fprintf(stderr,"error:%s\n",str);
}

int yywrap(){
    return 1;
}




