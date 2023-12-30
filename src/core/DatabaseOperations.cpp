#include "DatabaseOperations.h"
#include <iostream>
#include <filesystem>

// 构造函数
DatabaseOperations::DatabaseOperations() {
    tableOperations = nullptr;
}

// 析构函数
DatabaseOperations::~DatabaseOperations() {
    if (tableOperations != nullptr) {
        delete tableOperations;
    }
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
        if (tableOperations != nullptr) {
            delete tableOperations;
        }
        tableOperations = new TableOperations(currentDatabase);
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
