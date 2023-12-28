%{
    #include "stdio.h"
    #include "stdlib.h"
    #include "string.h"    
    int yylex();
    void yyerror(char* str);
%}

%union{
    int ivalue;
    char str[256];
}

%token <ivalue> num_INT 
%token <str> data_STRING ID KEY_type KEY_symbol
%token Y_STRING Y_INT
%token <ivalue> Y_LPAR Y_RPAR Y_EQ Y_SEMICOLON Y_ALL Y_COMMA
%token <ivalue> OP_CREATE OP_DROP OP_USE OP_SELECT OP_DELETE OP_INSERT OP_UPDATE OP_S_WHERE S_VALUES_ASSIS S_FROM S_SET N_TABLE N_DATABASE

%type <ivalue> DDL DDL_OP DML DML_OP SPE_STRU CONDITION DATASET INLIST DATA
%type <ivalue> TYPE OPE FINAL KEY DATALIST CONDITIONS


%%
FINAL:OPE
    |OPE FINAL

OPE:DDL {printf("DDL\n");}
   |DML {printf("DML\n");}

SPE_STRU:N_TABLE {}
        |N_DATABASE {printf("b\n");}

DDL:DDL_OP SPE_STRU ID Y_SEMICOLON {}
   |DDL_OP SPE_STRU ID Y_LPAR INLIST Y_RPAR Y_SEMICOLON {}
   |DDL_OP ID Y_SEMICOLON {}

DDL_OP:OP_CREATE {printf("a\n");}
      |OP_USE {}
      |OP_DROP {printf("a\n");}

DML:DML_OP DML_OBJ S_FROM ID OP_S_WHERE CONDITIONS Y_SEMICOLON{}
   |DML_OP DML_OBJ OP_S_WHERE CONDITIONS Y_SEMICOLON{}
   |DML_OP DML_OBJ S_VALUES_ASSIS Y_LPAR DATALIST Y_RPAR Y_SEMICOLON{}
   |DML_OP DML_OBJ S_SET CONDITION OP_S_WHERE CONDITIONS Y_SEMICOLON{}

DML_OP:OP_INSERT {}
      |OP_UPDATE {}
      |OP_DELETE {}
      |OP_SELECT {}

DML_OBJ:ID {}
       |Y_ALL {}

DATA:num_INT {}
    |data_STRING {printf("a\n");}

CONDITIONS:CONDITION
          |CONDITION Y_COMMA CONDITIONS
 
CONDITION:ID Y_EQ DATA {}

TYPE:Y_INT {}
    |Y_STRING {}

KEY:KEY_type KEY_symbol {printf("KEY_\n");}

DATASET:ID TYPE {}
       |ID TYPE KEY {}

DATALIST:DATA
        |DATA Y_COMMA DATALIST

INLIST:DATASET Y_COMMA INLIST {}
      |DATASET {}
%%
