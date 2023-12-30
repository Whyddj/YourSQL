#ifndef DATABASE_OPERATIONS_H
#define DATABASE_OPERATIONS_H

#include <string>

class DatabaseOperations {
public:
    DatabaseOperations();
    ~DatabaseOperations();

    void createDatabase(const std::string& dbName);
    void useDatabase(const std::string& dbName);
    void dropDatabase(const std::string& dbName);
};

#endif // DATABASE_OPERATIONS_H
