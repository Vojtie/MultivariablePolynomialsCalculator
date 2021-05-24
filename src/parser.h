/** @file
 * Interfejs analizatora składniowego kalkulatora wielomianów wielu zmiennych.
 *
 * @authors Wojciech Kuzebski <wk429552@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2021
 */


#ifndef POLYNOMIALS_PARSER_H
#define POLYNOMIALS_PARSER_H

#include <stddef.h>
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

/**
 * Wczytuje i zwraca następny
 * wiersz w postaci obiektu
 * struktury przechowującej
 * wiersz.
 * @return obiekt struktury przechowującej wiersz.
 */
Line GetNextLine();

#endif //POLYNOMIALS_PARSER_H
