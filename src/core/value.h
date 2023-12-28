#ifndef DBVALUE_H
#define DBVALUE_H
/*
数据定义
*/
enum value_type{
    VALUE_STRING=1,
    VALUE_INT=2
};

typedef struct value{
    int valueType;
    int length;
    union{
        int intValue;
        char*strValue;
    }rel_value;
} value;
#endif
