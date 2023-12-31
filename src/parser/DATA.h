#ifndef __DATA_H
#define __DATA_H

#include "stdio.h"
#include "stdlib.h"


union Data{
    int num_int;
    char str_data[256];
};

typedef struct Data_Type{
    union Data data;
    // char* name;
    int flag; // 0 for int, 1 for string
} Data_Type;

typedef struct Init_List{
    char* name;
    int flag;
    int type; // 1 for primary key, 0 for none
} Init_List;

enum Relation{
    EQ=1,GREAT=2,GREATEQ=3,LESS=4,LESSEQ=5
};

typedef struct Condition{
    char* name;
    enum Relation relation_op;
    Data_Type data;
} Condition;

#endif // __DATA_H