#ifndef TABLE_H
#define TABLE_H
#include "page.h"
#include"value.h"
#include"condition.h"
#include"sqlInfo.h"
#include"item.h"
#include <string>
#include<vector>
#include<memory>
#include<unordered_map>
#include<iostream>
//#include<algorithm>

class Table{
    private:
        using map = std::unordered_map<std::string, int>;
        meta_page metaPage;
        std::string fileName;
        std::vector<uint8_t> bufPtr;//文件读写缓冲
        std::string tableName;
        map colPos;//列名及其对应位置
    private:
        //不允许复制和赋值
        Table(const Table&) = delete;
        void operator=(const Table&) = delete;
    private:
        int getColPos(const std::string&colName);//获取字段的位置，从0开始
        int buildCondition(const ExprItem*condiExprPtr,std::unique_ptr<Condition>&conditionPtr,int *errorPos);//构建条件
        [[nodiscard]] std::unique_ptr<Condition> BuildIntCondition(int colPos, int op, int64_t val);
        [[nodiscard]] std::unique_ptr<Condition> BuildStringCondition(int colPos, int op, const std::string& val);
        void loadRecord(const std::vector<uint8_t>&recordPtr,std::vector<value>&recordVal);
        void clearPage(std::unique_ptr<data_page>&pagePtr);
    public:
        [[nodiscard]] const std::string& getTableName() const;
        void getColInfo(std::vector<col_info>&cols)const;
    public:
        Table();
        ~Table();
        int createTable(const std::string&tableName,const CreateInfo createInfo,int*errorPos);
        int openTable(const std::string &tableName,int*errorPos);
        int closeTable();
        int insert(const std::unique_ptr<InsertInfo> insertInfoPtr,int* errorPos);
        int query(const std::unique_ptr<QueryInfo> queryInfoPtr,int*errorPos );
        int delete_(const::std::unique_ptr<DeleteInfo>deleteInfoPtr,int*errorPos);
        

};
#endif