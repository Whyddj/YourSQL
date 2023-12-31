#include "DatabaseOperations.h"
#include <iostream>
#include <filesystem>

#define BASE_PATH "data/"

// 构造函数
DatabaseOperations::DatabaseOperations() {
    dataPageOps = nullptr;
    tableOps = new TableOperations();
}

// 析构函数
DatabaseOperations::~DatabaseOperations() {
    if (dataPageOps != nullptr) {
        delete dataPageOps;
    }
    delete tableOps;
}

void DatabaseOperations::openTable(const std::string& tableName) {
    if (dataPageOps != nullptr) {
        delete dataPageOps;
    }
    dataPageOps = new DataPageOps(tableName, currentDatabase);
}

bool DatabaseOperations::ifUseDatabase() {
    return currentDatabase != "";
}

// 创建数据库
void DatabaseOperations::createDatabase(const std::string& dbName) {
    std::string path = BASE_PATH + dbName;
    // 如果已经存在则不创建
    if (std::filesystem::exists(path)) {
        std::cerr << "Database already exists: " << dbName << std::endl;
        return;
    }
    // 创建数据库
    if (std::filesystem::create_directory(path)) {
        std::cout << "Database created: " << dbName << std::endl;
    } else {
        std::cerr << "Failed to create database: " << dbName << std::endl;
    }
}

// 使用数据库
void DatabaseOperations::useDatabase(const std::string& dbName) {
    std::string path = BASE_PATH + dbName;
    if (std::filesystem::exists(path)) {
        currentDatabase = dbName;
        // if (dataPageOps != nullptr) {
        //     delete dataPageOps;
        // }
        // dataPageOps = new dataPageOps(currentDatabase);
        std::cout << "Using database: " << dbName << std::endl;
    } else {
        std::cerr << "Database does not exist: " << dbName << std::endl;
    }
}

// 删除数据库
void DatabaseOperations::dropDatabase(const std::string& dbName) {
    std::string path = BASE_PATH + dbName;
    if (std::filesystem::remove_all(path) > 0) {
        std::cout << "Database dropped: " << dbName << std::endl;
    } else {
        std::cerr << "Failed to drop database: " << dbName << std::endl;
    }
}

// 显示所有数据库
void DatabaseOperations::showDatabases() {
    for (const auto& entry : std::filesystem::directory_iterator(BASE_PATH)) {
        if (entry.is_directory()) {
            std::cout << entry.path().filename().string() << std::endl;
        }
    }
}

// 创建表
void DatabaseOperations::createTable(const std::string& tableName,
                                     std::vector<Init_List> columns) {
    if (currentDatabase == "") {
        std::cerr << "No database is used" << std::endl;
        return;
    }
    tableOps->createTable(tableName, columns, currentDatabase);
    
}

// 删除表
void DatabaseOperations::dropTable(const std::string& tableName) {
    if (currentDatabase == "") {
        std::cerr << "No database is used" << std::endl;
        return;
    }
    tableOps->dropTable(tableName, currentDatabase);
}

// 显示所有表
void DatabaseOperations::showTables() {
    if (currentDatabase == "") {
        std::cerr << "No database is used" << std::endl;
        return;
    }
    // 遍历数据库目录所有db文件
    for (const auto& entry :
         std::filesystem::directory_iterator(BASE_PATH + currentDatabase)) {
        if (entry.is_regular_file()) {
            std::cout << entry.path().stem().string() << std::endl;
        }
    }
}

// 插入记录
void DatabaseOperations::insert(std::string tableName,
                                std::vector<Data_Type> record) {
    if (currentDatabase == "") {
        std::cerr << "No database is used" << std::endl;
        return;
    }
    if (tableName != usingTable) {
        if (dataPageOps != nullptr) {
            delete dataPageOps;
        }
        openTable(tableName);
        usingTable = tableName;
    }
    printf("222\n");
    dataPageOps->insertRecord(tableName, record);
}

// 查询记录
std::vector<std::map<std::string, std::string>> DatabaseOperations::selectRecord(const std::string &tableName, std::vector<std::string> &colNames, Condition &condition) {
    if (currentDatabase == "") {
        std::cerr << "No database is used" << std::endl;
        return std::vector<std::map<std::string, std::string>>();
    }
    if (tableName != usingTable) {
        if (dataPageOps != nullptr) {
            delete dataPageOps;
        }
        openTable(tableName);
        usingTable = tableName;
    }
    return dataPageOps->selectRecord(tableName, colNames, condition);
}

std::vector<std::map<std::string, std::string>> DatabaseOperations::selectAllRow(const std::string &tableName, std::vector<std::string> &colNames) {
    if (currentDatabase == "") {
        std::cerr << "No database is used" << std::endl;
        return std::vector<std::map<std::string, std::string>>();
    }
    if (tableName != usingTable) {
        if (dataPageOps != nullptr) {
            delete dataPageOps;
        }
        openTable(tableName);
        usingTable = tableName;
    }
    return dataPageOps->selectAllRow(tableName, colNames);
}

std::vector<std::map<std::string, std::string>> DatabaseOperations::selectAll(const std::string &tableName) {
    if (currentDatabase == "") {
        std::cerr << "No database is used" << std::endl;
        return std::vector<std::map<std::string, std::string>>();
    }
    if (tableName != usingTable) {
        if (dataPageOps != nullptr) {
            delete dataPageOps;
        }
        openTable(tableName);
        usingTable = tableName;
    }
    return dataPageOps->selectAll(tableName);
}

// 删除记录
void DatabaseOperations::deleteFrom(const std::string &tableName, Condition &condition) {
    if (currentDatabase == "") {
        std::cerr << "No database is used" << std::endl;
        return;
    }
    if (tableName != usingTable) {
        if (dataPageOps != nullptr) {
            delete dataPageOps;
        }
        openTable(tableName);
        usingTable = tableName;
    }
    dataPageOps->deleteRecord(tableName, condition);
}

void DatabaseOperations::deleteAll(const std::string &tableName) {
    if (currentDatabase == "") {
        std::cerr << "No database is used" << std::endl;
        return;
    }
    if (tableName != usingTable) {
        if (dataPageOps != nullptr) {
            delete dataPageOps;
        }
        openTable(tableName);
        usingTable = tableName;
    }
    dataPageOps->deleteAll(tableName);
}

// 更新记录
void DatabaseOperations::updateRecord(const std::string &tableName, Condition &columnCondition, Condition &condition) {
    if (currentDatabase == "") {
        std::cerr << "No database is used" << std::endl;
        return;
    }
    if (tableName != usingTable) {
        if (dataPageOps != nullptr) {
            delete dataPageOps;
        }
        openTable(tableName);
        usingTable = tableName;
    }
    dataPageOps->updateRecord(tableName, columnCondition, condition);
}

void DatabaseOperations::updateAll(const std::string &tableName, Condition &columnCondition) {
    if (currentDatabase == "") {
        std::cerr << "No database is used" << std::endl;
        return;
    }
    if (tableName != usingTable) {
        if (dataPageOps != nullptr) {
            delete dataPageOps;
        }
        openTable(tableName);
        usingTable = tableName;
    }
    dataPageOps->updateAll(tableName, columnCondition);
}