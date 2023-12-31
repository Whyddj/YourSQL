#ifndef __PARSE_OPERATIONS_H
#define __PARSE_OPERATIONS_H

#include "DATA.h"

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