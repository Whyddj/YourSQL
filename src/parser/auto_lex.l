%{
    #include "stdio.h"
    #include "stdlib.h"
    #include "string.h"
    #include "syntax.tab.h"
    #include "DATA.h"

    //[Ee][Xx][Ii][Tt] { return OP_EXIT; }
    //[Ss][Hh][Oo][Ww] { return OP_SHOW;}
    int yyparse();
%}

white [\n\t ]
ID [a-zA-Z]+
/* DATA_STR [a-zA-Z_0-9\,\.\+-=; ]+ */
INT [\+-]?[1-9][0-9]*


%%
{white} {}
{INT} {yylval.ivalue = atoi(yytext);return num_INT;}
[\+-]?0 {yylval.ivalue = 0;return num_INT;}
[Ss][Ee][Ll][Ee][Cc][Tt] { return OP_SELECT; }
[Ii][Nn][Ss][Ee][Rr][Tt] { return OP_INSERT; }
[Uu][Pp][Dd][Aa][Tt][Ee] { return OP_UPDATE; }
[Dd][Ee][Ll][Ee][Tt][Ee] { return OP_DELETE; }
[Cc][Rr][Ee][Aa][Tt][Ee] { return OP_CREATE; }
[Dd][Rr][Oo][Pp] { return OP_DROP; }
[Uu][Ss][Ee] { return OP_USE; }
[Ww][Hh][Ee][Rr][Ee] { return OP_S_WHERE; }
[Vv][Aa][Ll][Uu][Ee][Ss] { return S_VALUES_ASSIS; }
[Ff][Rr][Oo][Mm] { return S_FROM; }
[Ss][Ee][Tt] { return S_SET; }
[Tt][Aa][Bb][Ll][Ee] { return N_TABLE; }
[Dd][Aa][Tt][Aa][Bb][Aa][Ss][Ee] { return N_DATABASE; }

[Pp][Rr][Ii][Mm][Aa][Rr][Yy] { return KEY_type; }
[Kk][Ee][Yy] { return KEY_symbol; }
[Ii][Nn][Tt] { return Y_INT; }
[Vv][Aa][Rr][Cc][Hh][Aa][Rr] { return Y_STRING; }
"string" { return Y_STRING; }
{ID} {
    yylval.str = strdup(yytext); 
    return ID; 
    }



"\""+.+"\"" {return data_STRING;}
"(" {return Y_LPAR;}
")" {return Y_RPAR;}
";" {return Y_SEMICOLON;}
"-- "+.+\n {printf("注释:%s\n",yytext);}
"*" {return Y_ALL;}
"=" {return Y_EQ;}
"," {return Y_COMMA;}
">=" {return Y_GREATEQ;}
"<=" {return Y_LESSEQ;}
">" {return Y_GREAT;}
"<" {return Y_LESS;}


%%

void yyerror(char *str){
    fprintf(stderr,"error:%s\n",str);
    yyparse();
}

int yywrap(){
    return 1;
}

