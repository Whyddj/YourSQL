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
    KEY_type = 261,                /* KEY_type  */
    KEY_symbol = 262,              /* KEY_symbol  */
    Y_STRING = 263,                /* Y_STRING  */
    Y_INT = 264,                   /* Y_INT  */
    Y_LPAR = 265,                  /* Y_LPAR  */
    Y_RPAR = 266,                  /* Y_RPAR  */
    Y_EQ = 267,                    /* Y_EQ  */
    Y_SEMICOLON = 268,             /* Y_SEMICOLON  */
    Y_ALL = 269,                   /* Y_ALL  */
    Y_COMMA = 270,                 /* Y_COMMA  */
    OP_CREATE = 271,               /* OP_CREATE  */
    OP_DROP = 272,                 /* OP_DROP  */
    OP_USE = 273,                  /* OP_USE  */
    OP_SELECT = 274,               /* OP_SELECT  */
    OP_DELETE = 275,               /* OP_DELETE  */
    OP_INSERT = 276,               /* OP_INSERT  */
    OP_UPDATE = 277,               /* OP_UPDATE  */
    OP_S_WHERE = 278,              /* OP_S_WHERE  */
    S_VALUES_ASSIS = 279,          /* S_VALUES_ASSIS  */
    S_FROM = 280,                  /* S_FROM  */
    S_SET = 281,                   /* S_SET  */
    N_TABLE = 282,                 /* N_TABLE  */
    N_DATABASE = 283               /* N_DATABASE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 9 "syntax.y"

    int ivalue;
    char str[256];

#line 97 "syntax.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SYNTAX_TAB_H_INCLUDED  */
