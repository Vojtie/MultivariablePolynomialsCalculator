#ifndef POLYNOMIALS_POLY_PARSER_H
#define POLYNOMIALS_POLY_PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>

#include "poly.h"

typedef enum Operator {
    ZERO, IS_COEFF, IS_ZERO, CLONE, ADD, MUL, NEG, SUB, IS_EQ, DEG, DEG_BY, AT, PRINT, POP, NONE_OP
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
    WR_POLY, WR_COMMAND, WR_DEG_BY_VAR, WR_AT_VAL, ST_UND, NONE_ERR
} Error;

typedef enum LineType {
    POLY, OPER, EMPTY
} LineType;

typedef struct Line {
    char *chars;
    bool is_eof;
    Error error_type;
    size_t size;
    size_t last_index;
    LineType type;
    union {
        Poly p;
        Command c;
    };
} Line;

Line GetNextLine();

#endif //POLYNOMIALS_POLY_PARSER_H
