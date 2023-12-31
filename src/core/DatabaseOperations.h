#ifndef DATABASE_OPERATIONS_H
#define DATABASE_OPERATIONS_H

#include <string>
#include "DataPageOps.h"
#include "TableOperations.h"



class DatabaseOperations {
public:
    DatabaseOperations();
    ~DatabaseOperations();
    DataPageOps* dataPageOps;

    void createDatabase(const std::string& dbName);
    void useDatabase(const std::string& dbName);
    void dropDatabase(const std::string& dbName);
    void showDatabases();

    void openTable(const std::string& tableName);

    bool ifUseDatabase();

    void createTable(const std::string &tableName,
                   std::vector<Init_List> columns);

    void dropTable(const std::string &tableName);

    void showTables();

    void insert(std::string tableName, std::vector<Data_Type> record);

    std::vector<std::map<std::string, std::string>>
            selectRecord(const std::string &tableName, std::vector<std::string> &colNames,
                        Condition &condition);

    std::vector<std::map<std::string, std::string>>
            selectAllRow(const std::string &tableName,
                        std::vector<std::string> &colNames);

    std::vector<std::map<std::string, std::string>>
            selectAll(const std::string &tableName);

    void deleteFrom(const std::string &tableName, Condition &condition);

    void deleteAll(const std::string &tableName);

    void updateRecord(const std::string &tableName, Condition &columnCondition,
                    Condition &condition);

    void updateAll(const std::string &tableName, Condition &columnCondition);

private:
    std::string currentDatabase;
    TableOperations* tableOps;
    std::string usingTable;

};

#endif // DATABASE_OPERATIONS_H
