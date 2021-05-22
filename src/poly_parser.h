#ifndef POLYNOMIALS_POLY_PARSER_H
#define POLYNOMIALS_POLY_PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>

#include "poly.h"

#define EMPTY_LINE 0
#define POLY_LINE 1
#define OPER_LINE 2

typedef enum Operator {
    ZERO, IS_COEFF, IS_ZERO, CLONE, ADD, MUL, NEG, SUB, IS_EQ, DEG, DEG_BY, AT, PRINT, POP, NONE
} Operator;

typedef unsigned long deg_by_arg_t;

typedef long at_arg_t;

typedef struct Command {
    Operator op;
    union {
        at_arg_t at_arg;
        deg_by_arg_t deg_by_arg;
    };
} Command;

typedef enum Error {
    WR_POLY, WR_COMMAND, WR_DEG_BY_VAR, WR_AT_VAL, ST_UND
} Error;

typedef struct Line {
    char *chars;
    bool is_correct;
    bool is_eof;
    Error error_type;
    size_t size;
    size_t index;
    size_t type; // 1 - poly, 2 - operacja, 3 - pusty/ignorowany
} Line;

Command ReadCommand(Line *line);

Line ReadLine();

void PrintError(size_t line_num, Error error_type);

Poly StrToPoly(char **string);

void CheckLimits(Line *line);

#endif //POLYNOMIALS_POLY_PARSER_H
