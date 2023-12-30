/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SYNTAX_TAB_H_INCLUDED
# define YY_YY_SYNTAX_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 29 "syntax.y"

    // #include <iostream>
    // #include <string>
    // #include <vector>
    // #include <map>
    #include "DATA.h"
    #include "parse_operations.h"

#line 58 "syntax.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    num_INT = 258,                 /* num_INT  */
    data_STRING = 259,             /* data_STRING  */
    ID = 260,                      /* ID  */
    Y_EQ = 261,                    /* Y_EQ  */
    Y_GREAT = 262,                 /* Y_GREAT  */
    Y_GREATEQ = 263,               /* Y_GREATEQ  */
    Y_LESS = 264,                  /* Y_LESS  */
    Y_LESSEQ = 265,                /* Y_LESSEQ  */
    Y_STRING = 266,                /* Y_STRING  */
    Y_INT = 267,                   /* Y_INT  */
    KEY_type = 268,                /* KEY_type  */
    KEY_symbol = 269,              /* KEY_symbol  */
    Y_LPAR = 270,                  /* Y_LPAR  */
    Y_RPAR = 271,                  /* Y_RPAR  */
    Y_SEMICOLON = 272,             /* Y_SEMICOLON  */
    Y_ALL = 273,                   /* Y_ALL  */
    Y_COMMA = 274,                 /* Y_COMMA  */
    OP_CREATE = 275,               /* OP_CREATE  */
    OP_DROP = 276,                 /* OP_DROP  */
    OP_USE = 277,                  /* OP_USE  */
    OP_SELECT = 278,               /* OP_SELECT  */
    OP_DELETE = 279,               /* OP_DELETE  */
    OP_INSERT = 280,               /* OP_INSERT  */
    OP_UPDATE = 281,               /* OP_UPDATE  */
    OP_S_WHERE = 282,              /* OP_S_WHERE  */
    S_VALUES_ASSIS = 283,          /* S_VALUES_ASSIS  */
    S_FROM = 284,                  /* S_FROM  */
    S_SET = 285,                   /* S_SET  */
    N_TABLE = 286,                 /* N_TABLE  */
    N_DATABASE = 287               /* N_DATABASE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 38 "syntax.y"

    char** column_list;
    struct Data_Type** data_list;
    struct Init_List** init_list;
    struct Data_Type data;
    struct Init_List init;
    struct Condition condition;
    enum Relation relation_op;
    int ivalue;
    char *str;

#line 119 "syntax.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SYNTAX_TAB_H_INCLUDED  */
