#ifndef __DATA_H
#define __DATA_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <iostream>
#include <string>
union Data{
        int num_int;
        char str_data[256];
};

typedef struct Data_type* Data_List;

struct Data_type{
    char* name;
    union Data data;
    int flag;//定义类型时，flag表示是否是主键;定义插入的数据时表示数据类型
};

struct Init_List{
    char* name;
    int flag;//定义类型时，flag表示是否是主键;定义插入的数据时表示数据类型
    int type;//表示类型
};

enum Relation {
    EQ=1,GREAT=2,GREATEQ=3,LESS=4,LESSEQ=5
};

struct Condition{
    union Data data;
    char* name;
    int relation_op;
};

#endif
