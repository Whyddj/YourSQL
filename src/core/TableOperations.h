#ifndef TABLE_OPERATIONS_H
#define TABLE_OPERATIONS_H

#include <string>
#include <vector>
#include <map>

#define BASE_PATH "../data/"

enum class DataType {
    StringType,
    IntType
};

class TableOperations {
public:
    TableOperations(const std::string& dbName);
    ~TableOperations();

    void createTable(const std::string& tableName, const std::map<std::string, DataType>& columns, const std::string& primaryKey);
    void dropTable(const std::string& tableName);
    // void showTables();
    void selectFromTable(const std::string& tableName, const std::vector<std::string>& columns, const std::string& condition);
    void insertIntoTable(const std::string& tableName, const std::vector<std::string>& values);
    void deleteFromTable(const std::string& tableName, const std::string& condition);
    void updateTable(const std::string& tableName, const std::string& column, const std::string& value, const std::string& condition);

private:
    std::string currentDatabase;
};

#endif // TABLE_OPERATIONS_H
