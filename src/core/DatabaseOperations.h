#ifndef DATABASE_OPERATIONS_H
#define DATABASE_OPERATIONS_H

#include <string>
#include "TableOperations.h"



class DatabaseOperations {
public:
    DatabaseOperations();
    ~DatabaseOperations();
    TableOperations* tableOperations;

    void createDatabase(const std::string& dbName);
    void useDatabase(const std::string& dbName);
    void dropDatabase(const std::string& dbName);
    void showDatabases();

private:
    std::string currentDatabase;

};

#endif // DATABASE_OPERATIONS_H
