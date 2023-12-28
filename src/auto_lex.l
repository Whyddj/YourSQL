%{
    #include "stdio.h"
    #include "stdlib.h"
    #include "string.h"
    #include "syntax.tab.h"
    
    
%}

white [\n\t ]
ID [a-zA-Z]+
/* DATA_STR [a-zA-Z_0-9\,\.\+-=; ]+ */
INT [\+-]?[1-9][0-9]*


%%
{white} {}
{INT} {printf("%d\n",atoi(yytext));return num_INT;}
[\+-]?0 {printf("%d\n",atoi(yytext));return num_INT;}
{ID} {
        printf("%s\n",yytext);
        //数据类型声明
        if(strcmp(yytext,"INT")==0 || strcmp(yytext,"int") == 0) {return Y_INT;}
        else if(strcmp(yytext,"VARCHAR")==0 || strcmp(yytext,"varchar")==0 || strcmp(yytext,"string") == 0) {return Y_STRING;}
        //DDL
        else if(strcmp(yytext,"create")==0 || strcmp(yytext,"CREATE") == 0) {return OP_CREATE;}
        else if(strcmp(yytext,"drop")==0 || strcmp(yytext,"DROP") == 0) {return OP_DROP;}
        else if(strcmp(yytext,"use")==0 || strcmp(yytext,"USE") == 0) {return OP_USE;}
        //DML
        else if(strcmp(yytext,"select")==0 || strcmp(yytext,"SELECT") == 0) {return OP_SELECT;}
        else if(strcmp(yytext,"delete")==0 || strcmp(yytext,"DELETE") == 0) {return OP_DELETE;}
        else if(strcmp(yytext,"insert")==0 || strcmp(yytext,"INSERT") == 0) {return OP_INSERT;}
        else if(strcmp(yytext,"update")==0 || strcmp(yytext,"UPDATE") == 0) {return OP_UPDATE;}
        //assistance
        else if(strcmp(yytext,"where")==0 || strcmp(yytext,"WHERE") == 0) {return OP_S_WHERE;}
        else if(strcmp(yytext,"values")==0 || strcmp(yytext,"VALUES") == 0) {return S_VALUES_ASSIS;}
        else if(strcmp(yytext,"from")==0 || strcmp(yytext,"FROM") == 0) {return S_FROM;}
        else if(strcmp(yytext,"set")==0 || strcmp(yytext,"SET") == 0) {return S_SET;}
        //specific name
        else if(strcmp(yytext,"table")==0 || strcmp(yytext,"TABLE") == 0) {return N_TABLE;}
        else if(strcmp(yytext,"database")==0 || strcmp(yytext,"DATABASE") == 0) {return N_DATABASE;}
        else if(strcmp(yytext,"exit")==0 || strcmp(yytext,"EXIT") == 0) {printf("监测到退出提示,退出整个程序\n");exit(0);}
        else if(strcmp(yytext,"primary")==0 || strcmp(yytext,"PRIMARY") == 0){return KEY_type;}
        else if(strcmp(yytext,"key")==0 || strcmp(yytext,"KEY") == 0){return KEY_symbol;}
        else return ID;
    }



"\""+.+"\"" {printf("%s\n",yytext);return data_STRING;}
"(" {printf("%s\n",yytext);return Y_LPAR;}
")" {printf("%s\n",yytext);return Y_RPAR;}
";" {printf("%s\n",yytext);return Y_SEMICOLON;}
"-- "+.+\n {printf("注释:%s\n",yytext);}
"*" {printf("%s\n",yytext);return Y_ALL;}
"=" {printf("%s\n",yytext);return Y_EQ;}
"," {printf("%s\n",yytext);return Y_COMMA;}


%%

void yyerror(char *str){
    fprintf(stderr,"error:%s\n",str);
}

int yywrap(){
    return 1;
}

