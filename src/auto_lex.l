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
DATA_STR [a-zA-Z_0-9\,\.\+-=; ]+
INT [\+-]?[1-9][0-9]*


%%
{white} {}
{DATA_STR} {arr[index].Data = (char*)malloc(sizeof(yytext));strcpy(arr[index].Data,yytext);index++;return data_STRING;printf("%s\n",yytext);}
{INT} {arr[index++].ivalue == atoi(yytext);return num_INT;printf("%d\n",arr[index-1].ivalue);}
[\+-]?0 {arr[index++].ivalue = atoi(yytext);return num_INT;printf("%d\n",arr[index-1].ivalue);}
{ID} {
        arr[index].name = (char*)malloc(sizeof(yytext));strcpy(arr[index].name,yytext);index++;printf("%s\n",yytext);
        //数据类型声明
        if(strcmp(yytext,"INT")&&strcmp(yytext,"int") == 0) {return Y_INT;}
        if(strcmp(yytext,"VARCHAR")&&strcmp(yytext,"varchar")&&strcmp(yytext,"string") == 0) {return Y_STRING;}
        //DDL
        if(strcmp(yytext,"create")&&strcmp(yytext,"CREATE") == 0) {return OP_CREATE;}
        if(strcmp(yytext,"drop")&&strcmp(yytext,"DROP") == 0) {return OP_DROP;}
        if(strcmp(yytext,"use")&&strcmp(yytext,"USE") == 0) {return OP_USE;}
        //DML
        if(strcmp(yytext,"select")&&strcmp(yytext,"SELECT") == 0) {return OP_SELECT;}
        if(strcmp(yytext,"delete")&&strcmp(yytext,"DELETE") == 0) {return OP_DELETE;}
        if(strcmp(yytext,"insert")&&strcmp(yytext,"INSERT") == 0) {return OP_INSERT;}
        if(strcmp(yytext,"update")&&strcmp(yytext,"UPDATE") == 0) {return OP_UPDATE;}
        //assistance
        if(strcmp(yytext,"where")&&strcmp(yytext,"WHERE") == 0) {return S_WHERE;}
        if(strcmp(yytext,"values")&&strcmp(yytext,"VALUES") == 0) {return S_VALUES_ASSIS;}
        if(strcmp(yytext,"from")&&strcmp(yytext,"FROM") == 0) {return S_FROM;}
        if(strcmp(yytext,"set")&&strcmp(yytext,"SET") == 0) {return S_SET;}
        //specific name
        if(strcmp(yytext,"table")&&strcmp(yytext,"TABLE") == 0) {return N_TABLE;}
        if(strcmp(yytext,"database")&&strcmp(yytext,"DATABASE") == 0) {return N_DATABASE;}
        if(strcmp(yytext,"exit")&&strcmp(yytext,"EXIT") == 0) {printf("监测到退出提示,退出整个程序\n");exit(0);}
    }



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




