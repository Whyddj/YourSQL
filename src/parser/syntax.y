%{
    #include "stdio.h"
    #include "stdlib.h"
    #include "string.h"
    // #include <iostream>
    // #include <string>
    // #include <vector>
    // #include <map>
    #include "data.h"
    #include "parse_operations.h"

    // std::vector<std::string> ColumnVector;
    // std::vector<struct Data_type> DataVector;
    // std::vector<struct Init_List> TypeVector;
    char** ColumnVector;
    int size;
    struct Data_Type** DataVector;
    int dataSize;
    struct Init_List** TypeVector;
    int typeSize;

    struct Data_Type data;
    struct Init_List init;
    struct Condition condition;
    int yylex();
    void yyerror(const char* str);
%}

%code requires {
    // #include <iostream>
    // #include <string>
    // #include <vector>
    // #include <map>
    #include "data.h"
    #include "parse_operations.h"
}

%union{
    char** column_list;
    struct Data_Type** data_list;
    struct Init_List** init_list;
    struct Data_Type data;
    struct Init_List init;
    struct Condition condition;
    enum Relation relation_op;
    int ivalue;
    char str[255];
    char string_value[255];
}

%token <ivalue> num_INT 
%token <str> ID
%token <string_value> data_STRING
%token Y_EQ Y_GREAT Y_GREATEQ Y_LESS Y_LESSEQ
%token Y_STRING Y_INT KEY_type KEY_symbol
%token Y_LPAR Y_RPAR Y_SEMICOLON Y_ALL Y_COMMA
%token OP_CREATE OP_DROP OP_USE OP_SELECT OP_DELETE OP_INSERT OP_UPDATE OP_SHOW OP_S_WHERE S_VALUES_ASSIS S_FROM S_SET N_TABLE N_DATABASE EXIT N_TABLES N_DATABASES

%type DDL DML DML_OP
%type <condition> CONDITION 
%type <init>INITSET 
%type <init_list>INLIST
%type <data> DATA
%type <data_list> DATALIST
%type <column_list> COLUMN_LIST DML_OBJ
%type <relation_op> RELATION_OP
%type <ivalue> TYPE 
%type OPE FINAL KEY


%%
FINAL:OPE
    |OPE FINAL {}

OPE:DDL {}
   |DML {}
   |EXIT {exitDB();}

DDL:OP_CREATE N_DATABASE ID Y_SEMICOLON {createDB($3);}//创建数据库
   |OP_CREATE N_TABLE ID Y_LPAR INLIST Y_RPAR Y_SEMICOLON {
                                                        createTB($3,$5, typeSize);
                                                        for (int i = 0; i < typeSize; ++i) {
                                                            free(TypeVector[i]->name); // 释放每个元素指向的内存
                                                            free(TypeVector[i]); // 释放每个元素指向的内存
                                                        }
                                                        free(TypeVector); // 释放数组本身
                                                        TypeVector = NULL; // 避免悬挂指针
                                                        typeSize = 0;
                                                        }//创建表
   |OP_DROP N_DATABASE ID Y_SEMICOLON {dropDB($3);} //删除数据库
   |OP_DROP N_TABLE ID Y_SEMICOLON {dropTB($3);} //删除表
   |OP_USE ID Y_SEMICOLON {useDB($2);} //使用数据库

DML:DML_OP DML_OBJ S_FROM ID OP_S_WHERE CONDITION Y_SEMICOLON{
                                                            selectFromTB($2,$4,&$6,size);
                                                            for (int i = 0; i < size; ++i) {
                                                                free(ColumnVector[i]); // 释放每个元素指向的内存
                                                            }
                                                            free(ColumnVector); // 释放数组本身
                                                            ColumnVector = NULL; // 避免悬挂指针
                                                            size = 0;
                                                            }//从表中选择数据
   |DML_OP DML_OBJ S_FROM ID Y_SEMICOLON{
                            selectFromTB($2,$4,NULL,size);
                            for (int i = 0; i < size; ++i) {
                                free(ColumnVector[i]); // 释放每个元素指向的内存
                            }
                            free(ColumnVector); // 释放数组本身
                            ColumnVector = NULL; // 避免悬挂指针
                            size = 0;
                            } // 从表中选择所有数据
   |DML_OP DML_OBJ OP_S_WHERE CONDITION Y_SEMICOLON{deleteFromTB($2[0],&$4);} //从表中删除数据
   |DML_OP DML_OBJ S_VALUES_ASSIS Y_LPAR DATALIST Y_RPAR Y_SEMICOLON{
                                                            insertIntoTB($2[0],$5,dataSize);
                                                            for (int i = 0; i < dataSize; ++i) {
                                                                free(DataVector[i]); // 释放每个元素指向的内存
                                                            }
                                                            free(DataVector); // 释放数组本身
                                                            DataVector = NULL; // 避免悬挂指针
                                                            dataSize = 0;
                                                            }//向表中插入数据
   |DML_OP DML_OBJ S_SET CONDITION OP_S_WHERE CONDITION Y_SEMICOLON{updateTB($2[0],&$4,&$6);}//更新表中数据
    |OP_SHOW N_DATABASES Y_SEMICOLON{showDB();}//显示数据库
    |OP_SHOW N_TABLES Y_SEMICOLON{showTB();}//显示表

DML_OP:OP_INSERT 
      |OP_UPDATE 
      |OP_DELETE 
      |OP_SELECT 
      |OP_SHOW

COLUMN_LIST:ID {
                ColumnVector = (char**)realloc(ColumnVector, sizeof(char*) * (size + 1));
                ColumnVector[size++] = strdup($1);
                $$ = ColumnVector;
            }
           |ID Y_COMMA COLUMN_LIST {
                ColumnVector = (char**)realloc(ColumnVector, sizeof(char*) * (size + 1));
                ColumnVector[size++] = strdup($1);
                $$ = ColumnVector;
            }


DML_OBJ:COLUMN_LIST {$$ = $1;}
       |Y_ALL {size = 0;$$ = NULL;}


CONDITION:ID RELATION_OP DATA {condition.name = (char*)malloc(strlen($1) + 1);strcpy(condition.name, $1);condition.relation_op = $2;condition.data = $3;$$ = condition;
                                }

RELATION_OP:Y_EQ {$$ = EQ;}
           |Y_GREAT {$$ = GREAT;}
           |Y_GREATEQ {$$ = GREATEQ;}
           |Y_LESS {$$ = LESS;}
           |Y_LESSEQ {$$ = LESSEQ;}

TYPE:Y_INT {$$ = 0;}
    |Y_STRING {$$ = 1;}

KEY:KEY_type KEY_symbol

INITSET:ID TYPE {
        init.name = (char*)malloc(sizeof(strlen($1) + 1));
        strcpy(init.name,$1);
        init.flag=0;init.type=$2;
        $$ = init;
    }
       |ID TYPE KEY {
        init.name = (char*)malloc(sizeof(strlen($1) + 1));
        strcpy(init.name,$1);
        init.flag=1;init.type=$2;
        $$ = init;
    }

DATA:num_INT {data.data.num_int = $1;data.flag = 0;$$ = data;}
    |data_STRING {strcpy(data.data.str_data,$1);data.flag = 1;$$ = data;}

DATALIST:DATA {
            DataVector = (struct Data_Type**)realloc(DataVector, sizeof(struct Data_Type*) * (dataSize + 1));
            
            DataVector[dataSize] = (struct Data_Type*)malloc(sizeof(struct Data_Type));
            // printf("111");
            DataVector[dataSize]->data = $1.data;
            // DataVector[dataSize]->name = (char*)malloc(strlen($1.name) + 1);
            // strcpy(DataVector[dataSize]->name,$1.name);
            DataVector[dataSize]->flag = $1.flag;
            dataSize++; 
            $$ = DataVector;
            // printf("2%s\n",DataVector[dataSize-1]->name);
        }
        |DATA Y_COMMA DATALIST {
            DataVector = (struct Data_Type**)realloc(DataVector, sizeof(struct Data_Type*) * (dataSize + 1));
            
            DataVector[dataSize] = (struct Data_Type*)malloc(sizeof(struct Data_Type));
            // printf("111");
            DataVector[dataSize]->data = $1.data;
            // DataVector[dataSize]->name = (char*)malloc(strlen($1.name) + 1);
            // strcpy(DataVector[dataSize]->name,$1.name);
            DataVector[dataSize]->flag = $1.flag;
            dataSize++; 
            $$ = DataVector;
            // printf("2%s\n",DataVector[dataSize-1]->name);
        }

INLIST:INITSET Y_COMMA INLIST {
            TypeVector = (struct Init_List**)realloc(TypeVector, sizeof(struct Init_List*) * (typeSize + 1));
            TypeVector[typeSize] = (struct Init_List*)malloc(sizeof(struct Init_List));
            TypeVector[typeSize]->name = (char*)malloc(strlen($1.name) + 1);
            strcpy(TypeVector[typeSize]->name,$1.name);
            TypeVector[typeSize]->flag = $1.flag;
            TypeVector[typeSize]->type = $1.type;
            typeSize++;
            $$ = TypeVector;
        }
      |INITSET {
            TypeVector = (struct Init_List**)realloc(TypeVector, sizeof(struct Init_List*) * (typeSize + 1));
            TypeVector[typeSize] = (struct Init_List*)malloc(sizeof(struct Init_List));
            TypeVector[typeSize]->name = (char*)malloc(strlen($1.name) + 1);
            strcpy(TypeVector[typeSize]->name,$1.name);
            TypeVector[typeSize]->flag = $1.flag;
            TypeVector[typeSize]->type = $1.type;
            typeSize++;
            $$ = TypeVector;
        } 

%%
