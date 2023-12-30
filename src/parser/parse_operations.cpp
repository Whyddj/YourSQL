#include "parse_operations.h"
#include <iostream>

void createDB(char* DBname) {
    std::cout << "createDB" << std::endl;
}
void useDB(char* DBname) {
    std::cout << "useDB" << std::endl;
}
void dropDB(char* DBname) {
    std::cout << "dropDB" << std::endl;
}
void createTB(char* DBname,std::vector<struct Init_List> TypeVector) {
    std::cout << "createTB" << std::endl;
}
void dropTB(char* TBname) {
    std::cout << "dropTB" << std::endl;
}
void selectFromTB(char** ColumnVector,char* TBname,Condition* condition) {
    std::cout << "selectFromTB" << std::endl;
}
void insertIntoTB(char* TBname,Data_Type* DataVector) {
    std::cout << "insertIntoTB" << std::endl;
}
void deleteFromTB(char* TBname,Condition* condition) {
    std::cout << "deleteFromTB" << std::endl;
}
void updateTB(char* TBname,Condition* update,Condition* condition) {
    std::cout << "updateTB" << std::endl;
}
