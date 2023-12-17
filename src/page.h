#ifndef PAGE_H
#define PAGE_H
#include<cstdint>

#define DATA_PAGE_SIZE 4092
#define MAX_NAME_LEN 20
#define MAX_COL_NUM 15
/**
页类型定义
*/

//设置一页为4kb
typedef struct data_page{
    uint32_t pageNum;//页码
    uint8_t data[DATA_PAGE_SIZE];//u_int8保证每一个占八位一个字节
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
}meta_page ;

#endif