#ifndef __PARSE_OPERATIONS_H
#define __PARSE_OPERATIONS_H

// #include <iostream>
// #include <string>
// #include <vector>
// #include <map>
#include "DATA.h"
    


// void createDB(const std::string& DBname);
// void useDB(const std::string& DBname);
// void dropDB(const std::string& DBname);
// void createTB(const std::string& DBname,const std::vector<struct Init_List> TypeVector);
// void dropTB(const std::string& TBname);
// void selectFromTB(const std::vector<std::string> ColumnVector,const std::string& TBname,struct Condition condition);
// void insertIntoTB(const std::string& TBname,const std::vector<struct Data_Type> DataVector);
// void deleteFromTB(const std::string& TBname,struct Condition condition);
// void updateTB(const std::string& TBname,struct Condition update,struct Condition condition);

void createDB(char* DBname);
void useDB(char* DBname);
void dropDB(char* DBname);
void createTB(char* DBname,Init_List** TypeVector, int length);
void dropTB(char* TBname);
void selectFromTB(char** ColumnVector,char* TBname,Condition* condition, int length);
void insertIntoTB(char* TBname,Data_Type** DataVector, int length);
void deleteFromTB(char* TBname,Condition* condition);
void updateTB(char* TBname,Condition* update,Condition* condition);

void exitDB();
void showDB();
void showTB();

#endif