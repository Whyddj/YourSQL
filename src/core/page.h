#ifndef PAGE_H
#define PAGE_H
#include<cstdint>
#include<queue>
#include <map>
#include<vector>
#include<sstream>
#include<array>

#define DATA_PAGE_SIZE 4092
#define MAX_NAME_LEN 20
#define MAX_COL_NUM 20
/**
页类型定义
*/

struct ArrayHash {
    template <typename T, size_t N>
    std::size_t operator()(const std::array<T, N>& arr) const {
        std::hash<T> hasher;
        std::size_t result = 0;
        for (const auto& element : arr) {
            result = result * 31 + hasher(element);
        }
        return result;
    }
};

struct ArrayEqual {
    template <typename T, size_t N>
    bool operator()(const std::array<T, N>& lhs, const std::array<T, N>& rhs) const {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }
};

typedef struct data_page {
    uint32_t pageId; // 页码
    uint32_t isdeleted=0;//(1为已删除)
    std::map<std::array<char, MAX_NAME_LEN>, int, ArrayEqual> intValues;
    std::map<std::array<char, MAX_NAME_LEN>, std::vector<uint8_t>, ArrayEqual> stringValues;
} data_page;



typedef struct col_info{
    char colName[MAX_NAME_LEN];
    uint8_t coltype;
}col_info;

typedef struct meta_page{
    char tableName[MAX_NAME_LEN];
    uint32_t colNum;
    uint32_t rowNum;
    col_info colInfo[MAX_COL_NUM];
    std::queue<int> freePages;
    char primaryKey[MAX_NAME_LEN];
    std::map<std::array<char,MAX_NAME_LEN>,int,ArrayEqual> colInfoMap;
}meta_page ;

inline std::string serializeMetaPage(meta_page& data) {
    std::ostringstream oss;

    oss.write(data.tableName, sizeof(data.tableName));

    oss.write(reinterpret_cast<const char*>(&data.colNum), sizeof(uint32_t));
    oss.write(reinterpret_cast<const char*>(&data.rowNum), sizeof(uint32_t));
    for (size_t i = 0; i < MAX_COL_NUM; ++i) {
        oss.write(data.colInfo[i].colName, sizeof(data.colInfo[i].colName));
        oss.write(reinterpret_cast<const char*>(&data.colInfo[i].coltype), sizeof(uint8_t));
    }
    // 序列化队列大小
    size_t queueSize = data.freePages.size();
    oss.write(reinterpret_cast<const char*>(&queueSize), sizeof(size_t));

    // 序列化队列元素
    while (!data.freePages.empty()) {
        int element = data.freePages.front();
        oss.write(reinterpret_cast<const char*>(&element), sizeof(int));
        data.freePages.pop();
    }
    oss.write(data.primaryKey, sizeof(data.primaryKey));

   size_t Size = data.colInfoMap.size();
    oss.write(reinterpret_cast<const char*>(&Size), sizeof(size_t));
    for (const auto& entry : data.colInfoMap) {
        oss.write(reinterpret_cast<const char*>(&entry.first), sizeof(std::array<char, MAX_NAME_LEN>));
        oss.write(reinterpret_cast<const char*>(&entry.second), sizeof(int));
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
   for (size_t i = 0; i < MAX_COL_NUM; ++i) {
        iss.read(data.colInfo[i].colName, sizeof(data.colInfo[i].colName));
        iss.read(reinterpret_cast<char*>(&data.colInfo[i].coltype), sizeof(uint8_t));
    }

    // 反序列化队列大小
    size_t queueSize;
    iss.read(reinterpret_cast<char*>(&queueSize), sizeof(size_t));
    
    // 反序列化队列元素
    for (size_t i = 0; i < queueSize; ++i) {
        int element;
        iss.read(reinterpret_cast<char*>(&element), sizeof(int));
        data.freePages.push(element);
    }
    iss.read(data.primaryKey, sizeof(data.primaryKey));

    size_t Size;
    iss.read(reinterpret_cast<char*>(&Size), sizeof(size_t));
    for (size_t i = 0; i < Size; ++i) {
        std::array<char, MAX_NAME_LEN> key;
        int value;
        iss.read(reinterpret_cast<char*>(&key), sizeof(std::array<char, MAX_NAME_LEN>));
        iss.read(reinterpret_cast<char*>(&value), sizeof(int));
        data.colInfoMap[key] = value;
    }
}

inline std::string serializeDataPage(const data_page& data) {
    std::ostringstream oss;

    // 序列化 pageId
    oss.write(reinterpret_cast<const char*>(&data.pageId), sizeof(uint32_t));
    oss.write(reinterpret_cast<const char*>(&data.isdeleted), sizeof(uint32_t));

    // 序列化 intValues
    size_t intValuesSize = data.intValues.size();
    oss.write(reinterpret_cast<const char*>(&intValuesSize), sizeof(size_t));
    for (const auto& entry : data.intValues) {
        oss.write(reinterpret_cast<const char*>(&entry.first), sizeof(std::array<char, MAX_NAME_LEN>));
        oss.write(reinterpret_cast<const char*>(&entry.second), sizeof(int));
    }

    // 序列化 stringValues
    size_t stringValuesSize = data.stringValues.size();
    oss.write(reinterpret_cast<const char*>(&stringValuesSize), sizeof(size_t));
    for (const auto& entry : data.stringValues) {
        oss.write(reinterpret_cast<const char*>(&entry.first), sizeof(std::array<char, MAX_NAME_LEN>));

        size_t vectorSize = entry.second.size();
        oss.write(reinterpret_cast<const char*>(&vectorSize), sizeof(size_t));

        for (const auto& element : entry.second) {
            oss.write(reinterpret_cast<const char*>(&element), sizeof(uint8_t));
        }
    }

    // 填充至 4096 字节
    size_t totalSize = oss.str().size();
    size_t paddingSize = DATA_PAGE_SIZE - totalSize;

    for (size_t i = 0; i < paddingSize; ++i) {
        oss.put('\0');
    }

    return oss.str();
}

inline void deserializeDataPage(data_page& data, const std::string& serializedData) {
    std::istringstream iss(serializedData);

    // 反序列化 pageId
    iss.read(reinterpret_cast<char*>(&data.pageId), sizeof(uint32_t));
    iss.read(reinterpret_cast<char*>(&data.isdeleted), sizeof(uint32_t));
    // 反序列化 intValues
    size_t intValuesSize;
    iss.read(reinterpret_cast<char*>(&intValuesSize), sizeof(size_t));
    for (size_t i = 0; i < intValuesSize; ++i) {
        std::array<char, MAX_NAME_LEN> key;
        int value;
        iss.read(reinterpret_cast<char*>(&key), sizeof(std::array<char, MAX_NAME_LEN>));
        iss.read(reinterpret_cast<char*>(&value), sizeof(int));
        data.intValues[key] = value;
    }

    // 反序列化 stringValues
    size_t stringValuesSize;
    iss.read(reinterpret_cast<char*>(&stringValuesSize), sizeof(size_t));
    for (size_t i = 0; i < stringValuesSize; ++i) {
        std::array<char, MAX_NAME_LEN> key;
        iss.read(reinterpret_cast<char*>(&key), sizeof(std::array<char, MAX_NAME_LEN>));

        size_t vectorSize;
        iss.read(reinterpret_cast<char*>(&vectorSize), sizeof(size_t));

        std::vector<uint8_t> vectorData(vectorSize);
        for (size_t j = 0; j < vectorSize; ++j) {
            iss.read(reinterpret_cast<char*>(&vectorData[j]), sizeof(uint8_t));
        }

        data.stringValues[key] = vectorData;
    }
}
#endif