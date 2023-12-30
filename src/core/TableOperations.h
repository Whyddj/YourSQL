#ifndef TABLE_OPERATIONS_H
#define TABLE_OPERATIONS_H

#include "page.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include "DiskManager.h"
#include"DataPageOps.h"

#define CACHESIZE 2


class TableOperations {
    public:
    TableOperations(){};
    ~TableOperations(){};

    void createTable(const std::string& tableName, const std::map<std::string, DataType>& columns, const std::string& primaryKey)
    {   
        //加载元数据页
        std::string filename="../data/"+currentDatabase+tableName+".db";
        std::fstream file;
        file.open(filename, std::ios::out | std::ios::binary);
        meta_page metaPage={};
        std::strncpy(metaPage.tableName, tableName.c_str(), MAX_NAME_LEN - 1);
        if(!primaryKey.empty())
            std::strncpy(metaPage.primaryKey, primaryKey.c_str(), MAX_NAME_LEN-1);
            
        if (!file) {
            throw std::runtime_error("Unable to create file: " + filename);
            }

            // 填充4K的元数据区域
        std::vector<char> metadata(PAGE_SIZE, '\0'); // 使用'\0'初始化元数据区
        file.write(metadata.data(), PAGE_SIZE);
        if (!file) {
            throw std::runtime_error("Failed to write metadata to file: " + filename);
        }

        int colNum = columns.size();
       // std::cout << "Columns size: " << colNum << std::endl;
        metaPage.colNum = static_cast<uint32_t>(colNum);
        //std::cout << "metaPage.colNum: " << metaPage.colNum << std::endl;SS 
        metaPage.rowNum=static_cast<uint32_t>(0);
         
        //设置列字段信息
        for(int i=0;const auto& column:columns)
        {
            std::strncpy(metaPage.colInfo[i].colName,column.first.c_str(),MAX_NAME_LEN);
            if(column.second==DataType::StringType)
            {
                metaPage.colInfo[i].coltype= static_cast<uint8_t>(DataType::StringType);
            }else if(column.second==DataType::IntType)
            {
                metaPage.colInfo[i].coltype= static_cast<uint8_t>(DataType::IntType);
            }         
           i++;
        }
        
        
        file.seekg(0, std::ios_base::beg);
        std::string serializedMetaPage = serializeMetaPage(metaPage);
        file.write(serializedMetaPage.data(), serializedMetaPage.size());
        file.close();

        //创建索引文件
        std::string idxfilename="../data/"+currentDatabase+tableName+".idx";
        std::fstream idxfile;
        idxfile.open(idxfilename, std::ios::out | std::ios::binary);
        if (!idxfile) {
            throw std::runtime_error("Unable to create file: " + idxfilename);
        }
        idxfile.close();

    }
    void dropTable(const std::string& tableName)
    {
        std::string filename="../data/"+tableName+".db";
        std::string idxfilename="../data/"+tableName+".idx";
        if (std::remove(filename.c_str()) != 0) {
        perror("Error deleting file");
        } 
        std::remove(idxfilename.c_str());
    }

    void deleteAll(const std::string& tableName)
    {
        dropTable(tableName);
    }
    // void showTables();
    void selectFromTable(const std::string& tableName, const std::vector<std::string>& columns, const std::string& condition);
    void insertIntoTable(const std::string& tableName, const std::vector<std::string>& values);
    void deleteFromTable(const std::string& tableName, const std::string& condition);
    void updateTable(const std::string& tableName, const std::string& column, const std::string& value, const std::string& condition);
private:
    // currentDatabase
    std::string currentDatabase;
};

#endif // TABLE_OPERATIONS_H
