#ifndef __PARSE_OPERATIONS_H
#define __PARSE_OPERATIONS_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "stdio.h"
#include "DATA.h"

class parse {
public:
    parse();
    ~parse();

    void createDB(const std::string& DBname);
    void useDB(const std::string& DBname);
    void dropDB(const std::string& DBname);
    void createTB(const std::string& DBname,const std::vector<struct Init_List> TypeVector);
    void dropTB(const std::string& TBname);
    void selectFromTB(const std::vector<std::string> ColumnVector,const std::string& TBname,struct Condition condition);
    void insertIntoTB(const std::vector<std::string> ColumnVector,const std::vector<struct Data_Type> DataVector);
    void deleteFromTB(const std::vector<std::string> ColumnVector,struct Condition condition);
    void updateTB(const std::string& TBname,struct Condition update,struct Condition condition);
};

#endif