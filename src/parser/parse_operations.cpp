#include "parse_operations.h"
#include <iostream>
#include <stdio.h> 
#include <string.h>
#include "../util/util.h"

bool checkCondition(Condition* condition) {
    if (condition == NULL) {
        util::log(util::ERROR, "Condition is NULL");
        return false;
    }

    if (condition->relation_op != 1 && condition->data.flag == 1) {
        util::log(util::ERROR, "String can only be compared with =");
        return false;
    }

    std::string op;
    switch (condition->relation_op) {
        case 1:
            op = "=";
            break;
        case 2:
            op = ">";
            break;
        case 3:
            op = ">=";
            break;
        case 4:
            op = "<";
            break;
        case 5:
            op = "<=";
            break;
        default:
            op = "unknown";
            break;
    }


    util::log(util::INFO, "The condition is: ", condition->name, op, condition->data.flag == 0 ? std::to_string(condition->data.data.num_int) : condition->data.data.str_data);
    
    return true;
}

void createDB(char* DBname) {
    //char*转为string
    std::string _DBname(DBname);
    util::log(util::INFO, "createDB: ", _DBname);

    // TODO: 调用API
}
void useDB(char* DBname) {
    std::string _DBname(DBname);
    util::log(util::INFO, "useDB: ", _DBname);

    // TODO: 调用API
}
void dropDB(char* DBname) {
    std::string _DBname(DBname);
    util::log(util::INFO, "dropDB: ", _DBname);

    // TODO: 调用API
}
void createTB(char* DBname,Init_List** TypeVector, int length) {

    std::string _DBname(DBname);
    // Init_List** 转为 vector<Init_List>
    std::vector<Init_List> _TypeVector;
    for (int i = length - 1; i >= 0; i--) {
        Init_List temp;
        temp.name = strdup(TypeVector[i]->name);
        temp.flag = TypeVector[i]->flag;
        temp.type = TypeVector[i]->type;
        _TypeVector.push_back(temp);
        util::log(util::INFO, "createTB: ", temp.name, temp.flag == 0 ? "int" : "string", temp.type == 0 ? "none" : "primary key");

        // string不能作为primary key
        if (temp.flag == 1 && temp.type == 1) {
            util::log(util::ERROR, "String can not be primary key");
            return;
        }
    }
    // 释放内存
    for (int i = 0; i < length; i++) {
        free(TypeVector[i]->name);
        free(TypeVector[i]);
    }
    free(TypeVector);

    // TODO: 调用API
}
void dropTB(char* TBname) {
    std::string _TBname(TBname);
    util::log(util::INFO, "dropTB: ", _TBname);

    // TODO: 调用API
}
void selectFromTB(char** ColumnVector,char* TBname,Condition* condition, int length) {
    std::vector<std::string> _ColumnVector;
    for (int i = 0; i < length; i++) {
        std::string temp(ColumnVector[i]);
        _ColumnVector.push_back(temp);
    }
    util::log(util::INFO, "selectFromTB: ", TBname);
    for (int i = 0; i < length; i++) {
        util::log(util::INFO, "selectCol: ", _ColumnVector[i]);
    }
    if (!checkCondition(condition)) {
        return;
    }

    // TODO: 调用API
}

void insertIntoTB(char* TBname,Data_Type** DataVector, int length) {
    std::vector<Data_Type*> _DataVector;
    for (int i = 0; i < length; i++) {
        Data_Type* temp = (Data_Type*)malloc(sizeof(Data_Type));
        temp->flag = DataVector[i]->flag;
        if (temp->flag == 0) {
            temp->data.num_int = DataVector[i]->data.num_int;
        } else {
            strcpy(temp->data.str_data, DataVector[i]->data.str_data);
        }
        _DataVector.push_back(temp);
        util::log(util::INFO, "insertIntoTB: ", temp->flag == 0 ? std::to_string(temp->data.num_int) : temp->data.str_data);
    }

    // TODO: 调用API
}
void deleteFromTB(char* TBname,Condition* condition) {
    if (!checkCondition(condition)) {
        return;
    }

    util::log(util::INFO, "deleteFromTB: ", TBname);

    // TODO: 调用API
}
void updateTB(char* TBname,Condition* update,Condition* condition) {
    if (!checkCondition(update) || !checkCondition(condition)) {
        return;
    }

    util::log(util::INFO, "updateTB: ", TBname);
}

void exitDB() {
    util::log(util::INFO, "exitDB");
}

void showDB() {
    util::log(util::INFO, "showDB");
}

void showTB() {
    util::log(util::INFO, "showTB");
}