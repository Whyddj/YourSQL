#ifndef __DATA_h
#define __DATA_h

#include "stdio.h"
#include "stdlib.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>

union Data{
    int num_int;
    char str_data[256];
};

typedef struct Data_Type{
    union Data data;
    char* name;
    int flag;
} Data_Type;

typedef struct Init_List{
    char* name;
    int flag;
    int type;
} Init_List;

enum Relation{
    EQ=1,GREAT=2,GREATEQ=3,LESS=4,LESSEQ=5
};

typedef struct Condition{
    char* name;
    enum Relation relation_op;
    Data_Type data;
} Condition;

#endif