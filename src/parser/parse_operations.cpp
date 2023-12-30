#include "parse_operations.h"
#include <iostream>
#include <stdio.h> 
#include <string.h>

void createDB(char* DBname) {
    std::cout << "createDB" << std::endl;
    //char*转为string
    std::string _DBname(DBname);
}
void useDB(char* DBname) {
    std::cout << "useDB" << std::endl;
    std::string _DBname(DBname);
}
void dropDB(char* DBname) {
    std::cout << "dropDB" << std::endl;
    std::string _DBname(DBname);
}
void createTB(char* DBname,Init_List** TypeVector, int length) {
    // std::cout << "createTB" << std::endl;
    std::cout << "createTB :" << DBname << std::endl;
    for (int i = 0; i < length; i++) {
        // std::cout << "TypeVector[" << i << "]->name :" << TypeVector[i]->name << std::endl;
        printf("TypeVector[%d]->name :%s\n", i, TypeVector[i]->name);
        std::cout << "TypeVector[" << i << "]->flag :" << TypeVector[i]->flag << std::endl;
        std::cout << "TypeVector[" << i << "]->type :" << TypeVector[i]->type << std::endl;
    }

    std::string _DBname(DBname);
    // Init_List** 转为 vector<Init_List>
    std::vector<Init_List> _TypeVector;
    for (int i = 0; i < length; i++) {
        Init_List temp;
        temp.name = strdup(TypeVector[i]->name);
        temp.flag = TypeVector[i]->flag;
        temp.type = TypeVector[i]->type;
        _TypeVector.push_back(temp);
    }
    // 释放内存
    for (int i = 0; i < length; i++) {
        free(TypeVector[i]->name);
        free(TypeVector[i]);
    }
    free(TypeVector);
}
void dropTB(char* TBname) {
    std::cout << "dropTB" << std::endl;
    std::string _TBname(TBname);
}
void selectFromTB(char** ColumnVector,char* TBname,Condition* condition, int length) {
    std::cout << "selectFromTB" << std::endl;
    for (int i = 0; i < length; i++) {
        // std::cout << "ColumnVector[" << i << "] :" << ColumnVector[i] << std::endl;
        printf("ColumnVector[%d] :%s\n", i, ColumnVector[i]);
    }
}
void insertIntoTB(char* TBname,Data_Type** DataVector, int length) {
    std::cout << "insertIntoTB" << std::endl;
    for (int i = 0; i < length; i++) {
        // std::cout << "DataVector[" << i << "]->name :" << DataVector[i]->name << std::endl;
        // printf("DataVector[%d]->name :%s\n", i, DataVector[i]->name);
        std::cout << "DataVector[" << i << "]->flag :" << DataVector[i]->flag << std::endl;
        if (DataVector[i]->flag == 0)
            std::cout << "DataVector[" << i << "]->data :" << DataVector[i]->data.num_int << std::endl;
        else
            std::cout << "DataVector[" << i << "]->data :" << DataVector[i]->data.str_data << std::endl;
        // std::cout << "DataVector[" << i << "]->type :" << DataVector[i]->data << std::endl;
    }
}
void deleteFromTB(char* TBname,Condition* condition) {
    std::cout << "deleteFromTB" << std::endl;
    // std::cout << "condition->name :" << condition->name << std::endl;
    printf("condition->name :%s\n", condition->name);
    std::cout << "condition->relation_op :" << condition->relation_op << std::endl;
    std::cout << "condition->data.flag :" << condition->data.flag << std::endl;
}
void updateTB(char* TBname,Condition* update,Condition* condition) {
    std::cout << "updateTB" << std::endl;
}

void exitDB() {
    std::cout << "exitDB" << std::endl;
}

void showDB() {
    std::cout << "showDB" << std::endl;
}

void showTB() {
    std::cout << "showTB" << std::endl;
}