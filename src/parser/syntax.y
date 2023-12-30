%{
    #include "stdio.h"
    #include "stdlib.h"
    #include "string.h"
    #include <iostream>
    #include <string>
    #include <vector>
    #include <map>
    #include "DATA.h"
    #include "parse_operations.h"
    std::vector<std::string> ColumnVector;
    std::vector<struct Data_type> DataVector;
    std::vector<struct Init_List> TypeVector;
    struct Data_type data;
    struct Init_List init;
    struct Condition condition;
    enum Relation relation;
    parse* parse = new parse();
    int yylex();
    void yyerror(char* str);
%}

%code requires {
    #include <iostream>
    #include <string>
    #include <vector>
    #include <map>
    #include "DATA.h"
    #include "parse_operations.h"
}

%union{
    std::vector<std::string> column_list;
    std::vector<struct Data_type> data_list;
    std::vector<struct Init_List> init_list;
    struct Data_type data;
    struct Init_List init;
    struct Condition condition;
    enum Relation relation;
    int ivalue;
    char str[256];
}

%token <ivalue> num_INT 
%token <str> data_STRING ID
%token <relation> Y_EQ Y_GREAT Y_GREATEQ Y_LESS Y_LESSEQ
%token Y_STRING Y_INT KEY_type KEY_symbol
%token Y_LPAR Y_RPAR Y_SEMICOLON Y_ALL Y_COMMA
%token OP_CREATE OP_DROP OP_USE OP_SELECT OP_DELETE OP_INSERT OP_UPDATE OP_S_WHERE S_VALUES_ASSIS S_FROM S_SET N_TABLE N_DATABASE

%type DDL DML DML_OP 
%type <condition> CONDITION 
%type <data> INITSET 
%type <init>INLIST
%type <data> DATA
%type <data_list> DATALIST
%type <column_list> COLUMN_LIST DML_OBJ
%type <relation> RELATION_OP
%type <ivalue> TYPE 
%type OPE FINAL KEY


%%
FINAL:OPE
    |OPE FINAL

OPE:DDL {printf("DDL\n");}
   |DML {printf("DML\n");}

DDL:OP_CREATE N_DATABASE ID Y_SEMICOLON {parse->createDB($3);}//创建数据库
   |OP_CREATE N_TABLE ID Y_LPAR INLIST Y_RPAR Y_SEMICOLON {parse->createTB($3,$5);}//创建表
   |OP_DROP N_DATABASE ID Y_SEMICOLON {parse->dropDB($3);} //删除数据库
   |OP_DROP N_TABLE ID Y_SEMICOLON {parse->dropTB($3);} //删除表
   |OP_USE ID Y_SEMICOLON {parse->useDB($2);} //使用数据库

DML:DML_OP DML_OBJ S_FROM ID OP_S_WHERE CONDITION Y_SEMICOLON{parse->selectFromTB($2,$4,$6);}//从表中选择数据
   |DML_OP DML_OBJ OP_S_WHERE CONDITION Y_SEMICOLON{parse->deleteFromTB($2,$4);} //从表中删除数据
   |DML_OP DML_OBJ S_VALUES_ASSIS Y_LPAR DATALIST Y_RPAR Y_SEMICOLON{parse->insetIntoTB($2,$5);}//向表中插入数据
   |DML_OP DML_OBJ S_SET CONDITION OP_S_WHERE CONDITION Y_SEMICOLON{parse->updateTB($2,$4,$6);}//更新表中数据

DML_OP:OP_INSERT 
      |OP_UPDATE 
      |OP_DELETE 
      |OP_SELECT 

COLUMN_LIST:ID {ColumnVector.push_back($1);$$ = ColumnVector;}
           |ID Y_COMMA COLUMN_LIST {ColumnVector.push_back($1);$$ = ColumnVector;}

DML_OBJ:COLUMN_LIST {$$ = $1;}
       |Y_ALL {$$ = ColumnVector;}

DATA:num_INT {data.data.num_int = $1;data.flag = 0;$$ = data;}
    |data_STRING {strcpy(data.data.str_data,$1);data.flag = 1;$$ = data;}

CONDITION:ID RELATION_OP DATA {condition.name = (char*)malloc(sizeof(*$1));condition.relation_op = $2;condition.data = $3;$$ = condition;}

RELATION_OP:Y_EQ {$$ = 1;}
           |Y_GREAT {$$ = 2;}
           |Y_GREATEQ {$$ = 3;}
           |Y_LESS {$$ = 4;}
           |Y_LESSEQ {$$ = 5;}

TYPE:Y_INT {$$ = 0;}
    |Y_STRING {$$ = 1;}

KEY:KEY_type KEY_symbol

INITSET:ID TYPE {init.name = (char*)malloc(sizeof(*$1));strcpy(init.name,$1);init.flag=0;init.type=$2;TypeVector.push_back(init);}
       |ID TYPE KEY {init.name = (char*)malloc(sizeof(*$1));strcpy(init.name,$1);init.flag=1;init.type=$2;TypeVector.push_back(init);}

DATALIST:DATA {DataVector.push_back($1);$$ = DataVector;}
        |DATA Y_COMMA DATALIST {DataVector.push_back($1),$$ = DataVector;}

INLIST:INITSET Y_COMMA INLIST {TypeVector.push_back($1);$$ = TypeVector;}
      |INITSET {TypeVector.push_back($1);$$ = TypeVector;}
%%
