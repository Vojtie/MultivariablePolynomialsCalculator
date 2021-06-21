/** @file
 * Implementacja interfejsu analizatora składniowego kalkulatora wielomianów wielu zmiennych.
 *
 * @authors Wojciech Kuzebski <wk429552@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2021
 */

#ifndef POLYNOMIALS_PARSER_C
#define POLYNOMIALS_PARSER_C

#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "poly.h"


#define DEF_ALLOC_SIZE 32
#define DEF_ALLOC_COEFF 2

/**
 * Sprawdza czy alokacja pamięci się powiodła.
 * Jeśli nie to kończy program awaryjnie kodem 1.
 * @param[in] pointer : wskaźnik
 */
static void CheckAlloc(const void *pointer) {
  if (!pointer)
    exit(1);
}

/**
 * Zwiększa pamięć zaalokowaną
 * dla tablicy jednomianów @p monos.
 * @param monos : tablica jednomianów
 * @param size : rozmiar tablicy
 * @return wskaźnik na pierwszy element tablicy
 */
static Mono *IncreaseMemForMonos(Mono *monos, size_t *size) {
  *size *= DEF_ALLOC_COEFF;
  Mono *res = realloc(monos, *size * sizeof *res);
  CheckAlloc(res);
  return res;
}

/**
 * Konwertuje napis na współczynnik
 * wielomianu (stałą).
 * @param string : napis
 * @return współczynnik wielomianu
 */
poly_coeff_t StrToCoeff(char **string) {
  assert(string && *string);
  return strtol(*string, string, 10);
}

/**
 * Konwertuje napis na wykładnik
 * jednomianu.
 * @param string : napis
 * @return wykładnik
 */
poly_exp_t StrToExp(char **string) {
  assert(string && *string);
  return strtol(*string, string, 10);
}

/**
 * Konwertuje napis na wielomian.
 * @param string : napis
 * @return wielomian
 */
static Poly StrToPoly(char **string);

/**
 * Konwertuje napis na jednomian.
 * @param string : napis
 * @return jednomian
 */
static Mono StrToMono(char **string) {
  Mono res;
  (*string)++;
  res.p = StrToPoly(string);
  assert(**string == ',');
  (*string)++;
  assert((**string >= '0' && **string <= '9') || **string == '-');
  res.exp = StrToExp(string);
  assert(**string == ')');
  (*string)++;
  return res;
}

static Poly StrToPoly(char **string) {
  assert(string && *string);
  Poly res;
  if (**string == '(') {
    size_t size = 1, i = 0;
    Mono *monos = AllocMemForMonos(size);
    while (**string != '\0' && **string != ',') {
      assert(i == 0 || **string == '+');
      if (i > 0) {
        (*string)++;
        if (**string == '\0')
          break;
      }
      if (i == size)
        monos = IncreaseMemForMonos(monos, &size);
      monos[i++] = StrToMono(string);
    }
    if (i > 1) {
      res = PolyAddMonos(i, monos);
      free(monos);
    }
    else {
      assert(i == 1);
      res = PolyFromMonos(monos, i);
      res = PolyDelZeros(&res);
    }
    res = PolySimplify(&res);
  } else {
    assert((**string >= '0' && **string <= '9') || **string == '-');
    res = PolyFromCoeff(StrToCoeff(string));
  }
  return res;
}

/**
 * Sprawdza czy znak przedstawia liczbę.
 * @param ch : znak
 * @return Czy znak jest liczbą?
 */
static bool IsNumber(char ch) {
  return ch >= '0' && ch <= '9';
}

/**
 * Wczytuje z wiersza
 * argument polecenia DegBy.
 * @param line : wiersz
 * @param arg_i : położenie argumentu w wierszu
 * @return argument polecenia DegBy
 */
static deg_by_arg_t ReadDegByArg(Line *line, size_t arg_i) {
  deg_by_arg_t res;
  errno = 0;
  char *endptr = NULL;
  res = strtoul((char *) &line->chars[arg_i], &endptr, 10);
  if (errno == ERANGE || *endptr != '\0')
    line->error_type = WR_DEG_BY_VAR;
  return res;
}

/**
 * Wczytuje z wiersza
 * argument polecenia At.
 * @param line : wiersz
 * @param arg_i : położenie argumentu w wierszu
 * @return argument polecenia At
 */
static at_arg_t ReadAtArg(Line *line, size_t arg_i) {
  at_arg_t res;
  errno = 0;
  char *endptr = NULL;
  res = strtol((char *) &line->chars[arg_i], &endptr, 10);
  if (errno == ERANGE || *endptr != '\0')
    line->error_type = WR_AT_VAL;
  return res;
}

/**
 *
 * @param line
 * @param arg_i
 * @return
 */
static deg_by_arg_t ReadComposeArg(Line *line, size_t arg_i) {
  compose_arg_t res;
  errno = 0;
  char *endptr = NULL;
  res = strtoul((char *) &line->chars[arg_i], &endptr, 10);
  if (errno == ERANGE || *endptr != '\0')
    line->error_type = WR_COMPOSE_VAR;
  return res;
}
/**
 * Sprawdza czy napis składa
 * się jedynie z cyfr.
 * @param string : napis
 * @param size : długość napisu
 * @return Czy napis składa się jednie z cyfr?
 */
static bool AreDigits(char *string, size_t size) {
  bool res = true;
  for (size_t i = 0; i < size && res; i++)
    res &= IsNumber(string[i]);
  return res;
}

/**
 * Zwraca polecenie z wiersza
 * lub w przypadku niepowodzenia
 * ustawia odpowiedni błąd.
 * @param line : wiersz
 * @return polecenie
 */
static Command ReadCommand(Line *line) {
  assert(line->type == OPER);
  Command res;
  res.op = NONE_OP;
  char *oper = (char *) line->chars;
  if (line->last_index == 3 && oper[0] == 'A' && oper[1] == 'D' && oper[2] == 'D')
    res.op = ADD;
  else if (line->last_index == 3 && oper[0] == 'M' && oper[1] == 'U' && oper[2] == 'L')
    res.op = MUL;
  else if (line->last_index == 3 && oper[0] == 'N' && oper[1] == 'E' && oper[2] == 'G')
    res.op = NEG;
  else if (line->last_index == 3 && oper[0] == 'S' && oper[1] == 'U' && oper[2] == 'B')
    res.op = SUB;
  else if (line->last_index == 3 && oper[0] == 'D' && oper[1] == 'E' && oper[2] == 'G')
    res.op = DEG;
  else if (line->last_index == 3 && oper[0] == 'P' && oper[1] == 'O' && oper[2] == 'P')
    res.op = POP;
  else if (line->last_index == 4 && oper[0] == 'Z' && oper[1] == 'E' && oper[2] == 'R' && oper[3] == 'O')
    res.op = ZERO;
  else if (line->last_index >= 5 && oper[0] == 'I' && oper[1] == 'S' && oper[2] == '_') {
    if (line->last_index == 8 && oper[3] == 'C' && oper[4] == 'O' && oper[5] == 'E' && oper[6] == 'F' && oper[7] == 'F')
      res.op = IS_COEFF;
    else if (line->last_index == 7 && oper[3] == 'Z' && oper[4] == 'E' && oper[5] == 'R' && oper[6] == 'O')
      res.op = IS_ZERO;
    else if (line->last_index == 5 && oper[3] == 'E' && oper[4] == 'Q')
      res.op = IS_EQ;
    else
      line->error_type = WR_COMMAND;
  }
  else if (line->last_index >= 6 && oper[0] == 'D' && oper[1] == 'E' && oper[2] == 'G' && oper[3] == '_' && oper[4] == 'B' && oper[5] == 'Y') {
    if (line->last_index >= 8 && oper[6] == ' ' && AreDigits(oper + 7, line->last_index - 7)) {
      res.op = DEG_BY;
      res.deg_by_arg = ReadDegByArg(line, 7);
    }
    else
      line->error_type = WR_DEG_BY_VAR;
  }
  else if (line->last_index >= 2 && oper[0] == 'A' && oper[1] == 'T') {
    if (line->last_index >= 4 && oper[2] == ' ' && ((oper[3] == '-' && AreDigits(oper + 4, line->last_index - 4)) || AreDigits(oper + 3, line->last_index - 3))) {
      res.op = AT;
      res.at_arg = ReadAtArg(line, 3);
    } else
      line->error_type = WR_AT_VAL;
  }
  else if (line->last_index == 5 && oper[0] == 'P' && oper[1] == 'R' && oper[2] == 'I' && oper[3] == 'N' && oper[4] == 'T')
    res.op = PRINT;
  else if (line->last_index == 5 && oper[0] == 'C' && oper[1] == 'L' && oper[2] == 'O' && oper[3] == 'N' && oper[4] == 'E')
    res.op = CLONE;
  // zakladam ze brak spacji po compose to 'nie podano argumentu'
  else if (line->last_index >= 7 && oper[0] == 'C' && oper[1] == 'O' && oper[2] == 'M' && oper[3] == 'P' && oper[4] == 'O' && oper[5] == 'S' && oper[6] == 'E') {
    if (line->last_index >= 9 && oper[7] == ' ' && AreDigits(oper + 8, line->last_index - 8)) {
      res.op = COMPOSE;
      res.compose_arg = ReadComposeArg(line, 8);
    } else
      line->error_type = WR_COMPOSE_VAR;
  }
  else
    line->error_type = WR_COMMAND;
  return res;
}

/**
 * Sprawdza czy znak jest znakiem
 * oznaczającym wiersz z poleceniem.
 * @param ch
 * @return Czy znak oznacza wiersz z poleceniem?
 */
static bool IsOperLine(int ch) {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

/**
 * Sprawdza czy znak jest znakiem
 * oznaczającym wiersz ignorowany
 * lub pusty.
 * @param ch
 * @return czy znak jest znakiem
 * oznaczającym wiersz ignorowany
 * lub pusty?
 */
static bool IsIgnoredLine(int ch) {
  return ch == '#' || ch == EOF || ch =='\n';
}

/**
 * Sprawdza czy znak @p ch na podstawie
 * poprzedniego wczytanego znaku @p last_ch
 * jest poprawnym znakiem wiersza z wielomianem.
 * @param last_ch : ostatni wczytany znak
 * @param ch : znak
 * @return Czy znak jest poprawnym znakiem wiersza z wielomianem?
 */
static bool IsCorrectPolyChar(char last_ch, char ch) {
  bool res = true;
  if (last_ch == '(' && ch != '(' && !IsNumber(ch) && ch != '-')
    res = false;
  else if (last_ch == ',' && !IsNumber(ch) && ch != '-')
    res = false;
  else if (last_ch == ')' && ch != ',' && ch != '+')
    res = false;
  else if (last_ch == '-' && !IsNumber(ch))
    res = false;
  else if (IsNumber(last_ch) && !IsNumber(ch) && ch != ',' && ch != ')')
    res = false;
  else if (last_ch == '+' && ch != '(')
    res = false;
  return res;
}

/**
 * Sprawdza czy napis zawiera w
 * sobie poprawny zapis wielomianu.
 * @param str : napis
 * @param str_size : długość napisu
 * @return czy napis zawiera w sobie poprawny wielomian?
 */
static bool IsCorrectPoly(char *str, size_t str_size) {
  assert(str && str_size > 0);
  bool is_correct = true;
  if (IsNumber(str[0]) || (str[0] == '-' && str_size > 1)) {
    for (size_t i = 1; is_correct && i < str_size; i++)
      if (!IsNumber(str[i]))
        is_correct = false;
  }
  else if (str[0] == '(') {
    int parenths = 1;
    size_t cl_parenths = 0, commas = 0;
    for (size_t i = 1 ; is_correct && i < str_size; i++) {
      if (parenths < 0 || !IsCorrectPolyChar(str[i - 1], str[i]))
        is_correct = false;
      if (str[i] == '(') parenths++;
      else if (str[i] == ')') {
        cl_parenths++;
        parenths--;
      } else if (str[i] == ',')
        commas++;
    }
    if (parenths != 0 || cl_parenths != commas || str[str_size - 1] == '+')
      is_correct = false;
  }
  else is_correct = false;
  return is_correct;
}

/**
 * Zwraca nową strukturę
 * przechowującą wiersz.
 * @return obiekt struktury przechowującej wiersz
 */
static Line NewLine() {
  Line res;
  res.chars = malloc(DEF_ALLOC_SIZE * sizeof(*res.chars));
  res.size = DEF_ALLOC_SIZE;
  CheckAlloc(res.chars);
  res.last_index = 0;
  res.error_type = NONE_ERR;
  res.is_eof = false;
  return res;
}

/**
 * Zwiększa pamięć zaalokowaną
 * na obiekt struktury
 * przechowującej wiersz.
 * @param line : wiersz
 */
static void IncreaseLineSize(Line *line) {
  line->chars = realloc(line->chars, DEF_ALLOC_COEFF * line->size * sizeof(*line->chars));
  CheckAlloc(line->chars);
  line->size *= DEF_ALLOC_COEFF;
}

/**
 * Zwraca wczytany ze standardowego
 * wejścia wiersz.
 * @return wiersz
 */
static Line ReadLine() {
  Line res;
  int ch = getchar();
  if (IsIgnoredLine(ch)) {
    res.type = EMPTY;
    res.error_type = NONE_ERR;
    while (ch!= EOF && ch != '\n')
      ch = getchar();
  }
  else {
    res = NewLine();
    if (IsOperLine(ch))
      res.type = OPER;
    else res.type = POLY;
    res.chars[res.last_index++] = (char) ch;
    while ((ch = getchar()) != EOF && ch != '\n') {
      res.chars[res.last_index++] = (char) ch;
      if (res.last_index == res.size)
        IncreaseLineSize(&res);
    }
    res.chars[res.last_index] = '\0';
  }
  res.is_eof = ch == EOF ? true : false;
  return res;
}

/**
 * Sprawdza czy napis zawiera
 * w sobie dozwolone liczby.
 * @param str : napis
 * @param str_size : długość napisu
 * @return Czy napis zawiera poprawne liczby?
 */
static bool HasCorrectNumbers(char *str, size_t str_size) {
  errno = 0;
  bool correct = true;
  if (IsNumber(str[0]) || str[0] == '-') {
    strtol(str, NULL, 10);
    if (errno == ERANGE)
      correct = false;
  }
  for (size_t i = 0; correct && i < str_size - 1; i++) {
    if (str[i] == '(' && (str[i + 1] == '-' || IsNumber(str[i + 1]))) {
      strtol(&str[i + 1], NULL, 10);
      if (errno == ERANGE)
        correct = false;
    }
    else if (str[i] == ',') {
      long exp = strtol(&str[i + 1], NULL, 10);
      if (exp > INT_MAX || exp < 0) {
        correct = false;
      }
    }
  }
  return correct;
}

Line GetNextLine() {
  Line res = ReadLine();
  char *str = res.chars;
  if (res.type == POLY) {
    if (IsCorrectPoly(str, res.last_index) && HasCorrectNumbers((char *) str, res.last_index))
      res.p = StrToPoly((char **) &str);
    else
      res.error_type = WR_POLY;
  }
  else if (res.type == OPER)
    res.c = ReadCommand(&res);
  return res;
}

#endif //POLYNOMIALS_POLY_PARSER_C