#ifndef PAGE_H
#define PAGE_H
#include<cstdint>
#include<queue>
#include <unordered_map>
#include <vector>
#include<sstream>

#define DATA_PAGE_SIZE 4092
#define MAX_NAME_LEN 20
#define MAX_COL_NUM 20
/**
页类型定义
*/

typedef struct data_page{
    uint32_t pageId;//页码
    std::unordered_map<const char[MAX_NAME_LEN], int> intValues;
    std::unordered_map<const char[MAX_NAME_LEN], std::vector<uint8_t>> stringValues;
}data_page ;

typedef struct col_info{
    char colName[MAX_NAME_LEN];
    uint8_t coltype;
    uint8_t isPrimaryKey;
}col_info;

typedef struct meta_page{
    char tableName[MAX_NAME_LEN];
    uint32_t colNum;
    uint32_t rowNum;
    col_info colInfo[MAX_COL_NUM];
    std::queue<int> freePages;
}meta_page ;

inline std::string serializeMetaPage(meta_page& data) {
    std::ostringstream oss;

    oss.write(data.tableName, sizeof(data.tableName));

    oss.write(reinterpret_cast<const char*>(&data.colNum), sizeof(uint32_t));
    oss.write(reinterpret_cast<const char*>(&data.rowNum), sizeof(uint32_t));
    oss.write(reinterpret_cast<const char*>(&data.colInfo), sizeof(data.colInfo));

    // 序列化队列大小
    size_t queueSize = data.freePages.size();
    oss.write(reinterpret_cast<const char*>(&queueSize), sizeof(size_t));

    // 序列化队列元素
    while (!data.freePages.empty()) {
        int element = data.freePages.front();
        oss.write(reinterpret_cast<const char*>(&element), sizeof(int));
        data.freePages.pop();
    }
    
    const size_t remainingBytes = 4096 - oss.str().size();
    for (size_t i = 0; i < remainingBytes; ++i) {
        oss.put('\0');
    }
    return oss.str();
}

inline void deserializeMetaPage(meta_page& data, const std::string& serializedData) {
    std::istringstream iss(serializedData);

    iss.read(data.tableName, sizeof(data.tableName));
    iss.read(reinterpret_cast<char*>(&data.colNum), sizeof(uint32_t));
    iss.read(reinterpret_cast<char*>(&data.rowNum), sizeof(uint32_t));
    iss.read(reinterpret_cast<char*>(&data.colInfo), sizeof(data.colInfo));

    // 反序列化队列大小
    size_t queueSize;
    iss.read(reinterpret_cast<char*>(&queueSize), sizeof(size_t));

    // 反序列化队列元素
    for (size_t i = 0; i < queueSize; ++i) {
        int element;
        iss.read(reinterpret_cast<char*>(&element), sizeof(int));
        data.freePages.push(element);
    }
}

#endif