#ifndef POLYNOMIALS_POLY_PARSER_C
#define POLYNOMIALS_POLY_PARSER_C

#include <stdlib.h>
#include <stdio.h>
#include "poly.h"

#define DEF_ALLOC_SIZE 10
#define DEF_ALLOC_COEFF 2
#define EMPTY_LINE 0
#define POLY_LINE 1
#define OPER_LINE 2

static void CheckAlloc(const void *pointer) {
  if (!pointer)
    exit(1);
}

static Mono *AllocMemForMonos(size_t count) {
  Mono *res = calloc(count, sizeof(*res));
  CheckAlloc(res);
  return res;
}

static Mono *IncreaseMemForMonos(Mono *monos, size_t *size) {
  *size *= DEF_ALLOC_COEFF;
  Mono *res = realloc(monos, *size * sizeof *res);
  CheckAlloc(res);
  return res;
}
/*
typedef enum Operator {
    ADD;
} Op;
*/
typedef struct Line {
    char *chars;
    size_t size;
    size_t index;
    size_t type; // 1 - poly, 2 - operacja, 3 - pusty/ignorowany
} Line;

poly_coeff_t StrToCoeff(char **string) {
  assert(string && *string);
  return strtol(*string, string, 10);
}

poly_exp_t StrToExp(char **string) {
  assert(string && *string);
  return strtol(*string, string, 10);
}

static Poly StrToPoly(char **string);

static Mono StrToMono(char **string) {
  //assert(string && *string && **string == '(');
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
/**
 * wywala się na (1,0)+(1,2) chyba bo przerywa na ','
 * ((1,1),0)+(1,2)
 *
 **/
static Poly StrToPoly(char **string) {
  assert(string && *string);
  Poly res;
  if (**string == '(') {
    size_t size = 1;
    Mono *monos = AllocMemForMonos(size);
    size_t i = 0;
    while (**string != '\0' && **string != ',') {
      assert(i == 0 || **string == '+');
      if (i > 0) {
        (*string)++;
        if (**string == '\0')
          break;
      }
      if (i == size)
        monos = IncreaseMemForMonos(monos, &size);
      monos[i] = StrToMono(string);
      i++;
    }
    if (i > 1) {
      res = PolyAddMonos(i, monos);
      free(monos);
    }
    else {
      // size = 1
      assert(i == 1);
      res = PolyFromMonos(monos, i);
      res = PolyDelZeros(&res);
    }
  } else {
    assert((**string >= '0' && **string <= '9') || **string == '-');
    res = PolyFromCoeff(StrToCoeff(string));
  }
  return res;
}

static Line AllocMemForLine() {
  Line res;
  res.chars = calloc(DEF_ALLOC_SIZE, sizeof res.chars);
  CheckAlloc(res.chars);
  res.size = DEF_ALLOC_SIZE;
  res.type = EMPTY_LINE;
  res.index = 0;
  return res;
}

static void IncreaseLineSize(Line *line) {
  line->size *= DEF_ALLOC_COEFF;
  line->chars = realloc(line->chars, sizeof(line->chars) * line->size);
  CheckAlloc(line->chars);
}

static Line ReadLine(size_t line_num) {
  char ch;
  Line line = AllocMemForLine();
  while ((ch = (char) getchar()) != EOF && ch != '\n') {
    if (line.index == 0) {
      if (ch == '#') {
        line.type = EMPTY_LINE;
        break;
      }
      else if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        line.type = OPER_LINE;
      else
        line.type = POLY_LINE;
    }
    else {
      // poprawność (czy jeden wiel, nawiasy, czy dopuszczalne znaki, errory
    }
    if (line.index == line.size - 1)
      IncreaseLineSize(&line);
    line.chars[line.index++] = ch;
  }
  line.chars[line.index] = '\0';
  puts(line.chars);
  return line;
}

int main() {
  Line input = ReadLine(0);
  char **string = &input.chars;
  Poly p = StrToPoly(string);
  PolyDestroy(&p);
  free(input.chars);
  return 0;
}

#endif //POLYNOMIALS_POLY_PARSER_C