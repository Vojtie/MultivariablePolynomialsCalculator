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

/**
 * To jest typ wyliczeniowy oznaczający polecenie.
 */
typedef enum Operator {
    ZERO, IS_COEFF, IS_ZERO, CLONE, ADD, MUL, NEG, SUB, IS_EQ, DEG, DEG_BY, AT, PRINT, POP, COMPOSE, NONE_OP
} Operator;

/** To jest typ reprezentujący argument polecenia DegBy. */
typedef unsigned long deg_by_arg_t;

/** To jest typ reprezentujący argument polecenia Compose. */
typedef unsigned long compose_arg_t;

/** To jest typ reprezentujący argument polecenia At. */
typedef long at_arg_t;

/**
 * To jest struktura przechowująca polecenie wraz z jego potencjalnym argumentem.
 */
typedef struct Command {
    /** To jest obiekt przechowujący polecenie */
    Operator op;
    union {
        /** To jest zmienna przechowująca argument polecenia AT */
        at_arg_t at_arg;
        /** To jest zmienna przechowująca argument polecenia DEG_BY */
        deg_by_arg_t deg_by_arg;
        /** To jest zmienna przechowująca argument polecenia COMPOSE */
        compose_arg_t compose_arg;
    };
} Command;

/**
 * To jest typ wyliczeniowy oznaczający błąd.
 */
typedef enum Error {
    WR_POLY, WR_COMMAND, WR_DEG_BY_VAR, WR_COMPOSE_VAR, WR_AT_VAL, ST_UND, NONE_ERR
} Error;

/**
 * To jest typ wyliczeniowy oznaczający typ wiersza.
 */
typedef enum LineType {
    POLY, OPER, EMPTY
} LineType;

/**
 * To jest struktura przechowująca wiersz.:
 */
typedef struct Line {
    /** To jest tablica przechowująca wiersz */
    char *chars;
    /** To jest zmienna typu logicznego przechowująca informację czy wczytany wiersz jest ostatni w pliku */
    bool is_eof;
    /** To jest obiekt przechowujący typ błędu */
    Error error_type;
    /** To jest zmienna przechowująca rozmiar wiersza */
    size_t size;
    /** To jest zmienna przechowująca numer ostatniego indeksu wiersza */
    size_t last_index;
    /** To jest obiekt przechowujący typ wiersza */
    LineType type;
    /** To jest unia przechowująca wielomian lub polecenie */
    union {
        /** To jest wielomian */
        Poly p;
        /** To jest polecenie */
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
