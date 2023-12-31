#include "parse_operations.h"
#include <iostream>
#include <stdio.h> 
#include <string.h>
#include "../util/util.h"
#include "../core/DatabaseOperations.h"

DatabaseOperations* dbOps = new DatabaseOperations();

bool checkCondition(Condition* condition, bool isSelect = false) {
    if (condition == NULL && !isSelect) {
        util::log(util::ERROR, "Condition is NULL");
        return false;
    }
    if (isSelect && condition == NULL) {
        util::log(util::INFO, "Select without condition");
        return true;
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

void printDatabaseResult(const std::vector<std::map<std::string, std::string>>& result) {
    for (const auto& row : result) {
        for (const auto& pair : row) {
            std::cout << pair.first << ": " << pair.second << "  ";
        }
        std::cout << std::endl;
    }
}

void createDB(char* DBname) {
    //char*转为string
    std::string _DBname(DBname);
    util::log(util::INFO, "createDB: ", _DBname);

    dbOps->createDatabase(_DBname);
}
void useDB(char* DBname) {
    std::string _DBname(DBname);
    util::log(util::INFO, "useDB: ", _DBname);

    dbOps->useDatabase(_DBname);
}
void dropDB(char* DBname) {
    std::string _DBname(DBname);
    util::log(util::INFO, "dropDB: ", _DBname);

    dbOps->dropDatabase(_DBname);
    
}
void createTB(char* DBname,Init_List** TypeVector, int length) {
    bool hasPKey = false;

    std::string _DBname(DBname);
    // Init_List** 转为 vector<Init_List>
    // 打印Init_List
    // for (int i = 0; i < length; i++) {
    //     Init_List temp;
    //     temp.name = strdup(TypeVector[i]->name);
    //     temp.flag = TypeVector[i]->flag;
    //     temp.type = TypeVector[i]->type;
    //     util::log(util::INFO, "createTB: ", temp.name, "\t" , temp.flag == 0 ? "int" : "string","\t", temp.type == 0 ? "none" : "primary key");
    // }


    std::vector<Init_List> _TypeVector;
    for (int i = length - 1; i >= 0; i--) {
        Init_List temp;
        temp.name = strdup(TypeVector[i]->name);
        temp.flag = TypeVector[i]->flag;
        temp.type = TypeVector[i]->type;
        _TypeVector.push_back(temp);
        util::log(util::INFO, "createTB: ", temp.name, "\t" , temp.flag == 0 ? "int" : "string","\t", temp.type == 0 ? "none" : "primary key");

        // string不能作为primary key
        if (temp.flag == 1 && temp.type == 1) {
            util::log(util::ERROR, "String can not be primary key");
            return;
        }

        if (temp.type == 1) {
            if (hasPKey) {
                util::log(util::ERROR, "Only one primary key is allowed");
                return;
            } else {
                hasPKey = true;
            }
        }

    }

    if (!dbOps->ifUseDatabase()) {
        util::log(util::ERROR, "No database is used");
        return;
    }
    // TODO: 调用API
    // 打印_TypeVector
    for (int i = 0; i < _TypeVector.size(); i++) {
        util::log(util::INFO, "createTB: ", _TypeVector[i].name, "\t" , _TypeVector[i].flag ,"\t", _TypeVector[i].type);
    }

    dbOps->createTable(_DBname, _TypeVector);
}
void dropTB(char* TBname) {
    std::string _TBname(TBname);
    util::log(util::INFO, "dropTB: ", _TBname);

    // TODO: 调用API

    dbOps->dropTable(_TBname);
}
void selectFromTB(char** ColumnVector,char* TBname,Condition* condition, int length) {
    std::vector<std::string> _ColumnVector;
    if (ColumnVector == NULL) {
        for (int i = 0; i < length; i++) {
            std::string temp(ColumnVector[i]);
            _ColumnVector.push_back(temp);
        }
    }
    util::log(util::INFO, "selectFromTB: ", TBname);
    if (!checkCondition(condition, true)) {
        return;
    }

    std::vector<std::map<std::string, std::string>> result;
    
    if ((ColumnVector == NULL || length == 0)) {
        util::log(util::INFO, "selectCol: *");
        result = dbOps->selectAll(TBname);
        printDatabaseResult(result);
        return;
    }
    if (condition == NULL) {
        util::log(util::INFO, "selectCol: *");
        result = dbOps->selectAllRow(TBname, _ColumnVector);
        printDatabaseResult(result);
        return;
    }
    
    for (int i = 0; i < length; i++) {
        util::log(util::INFO, "selectCol: ", _ColumnVector[i]);
        return;
    }

    result = dbOps->selectRecord(TBname, _ColumnVector, *condition);
    printDatabaseResult(result);
}

void insertIntoTB(char* TBname,Data_Type** DataVector, int length) {
    std::vector<Data_Type> _DataVector;
    for (int i = 0; i < length; i++) {
        Data_Type temp;

        temp.flag = DataVector[i]->flag;
        if (temp.flag == 0) {
            temp.data.num_int = DataVector[i]->data.num_int;
        } else {
            strcpy(temp.data.str_data, DataVector[i]->data.str_data);
        }

        _DataVector.push_back(temp);
        // util::log(util::INFO, "insertIntoTB: ", temp.flag == 0 ? std::to_string(temp.data.num_int) : temp.data.str_data);
    }
    printf("1111\n");
    // 打印_DataVector
    // for (int i = 0; i < _DataVector.size(); i++) {
    //     util::log(util::INFO, "insertIntoTB: ", _DataVector[i].flag );
    // }

    dbOps->insert(TBname, _DataVector);
}
void deleteFromTB(char* TBname,Condition* condition) {
    if (!checkCondition(condition)) {
        return;
    }

    util::log(util::INFO, "deleteFromTB: ", TBname);

    // TODO: 调用API
    dbOps->deleteFrom(TBname, *condition);
}
void updateTB(char* TBname,Condition* update,Condition* condition) {
    if (!checkCondition(update) || !checkCondition(condition)) {
        return;
    }

    util::log(util::INFO, "updateTB: ", TBname);

    dbOps->updateRecord(TBname, *update, *condition);
}

void exitDB() {
    util::log(util::INFO, "exitDB");
    dbOps->~DatabaseOperations();
    std::cout << "Bye!" << std::endl;
    exit(0);
}

void showDB() {
    util::log(util::INFO, "showDB");

    dbOps->showDatabases();
}

void showTB() {
    util::log(util::INFO, "showTB");

    dbOps->showTables();
}