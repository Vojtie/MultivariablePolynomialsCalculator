/** @file
  Implementacja kalkulatora działającego na wielomianach rzadkich wielu zmiennych.

  @authors Wojciech Kuzebski <wk429552@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_CALC_C
#define POLYNOMIALS_CALC_C

#include <stdio.h>
#include <stdlib.h>
#include "calc.h"

/**
 * Zmienna przechowująca numer
 * aktualnie przetwarzanego wiersza.
 */
static size_t line_num = 1;

static void PrintError(Error error_type) {
  switch (error_type) {
    case WR_POLY:
      fprintf(stderr, "ERROR %zu WRONG POLY\n", line_num);
      break;
    case WR_COMMAND:
      fprintf(stderr, "ERROR %zu WRONG COMMAND\n", line_num);
      break;
    case WR_DEG_BY_VAR:
      fprintf(stderr, "ERROR %zu DEG BY WRONG VARIABLE\n", line_num);
      break;
    case WR_COMPOSE_VAR:
      fprintf(stderr, "ERROR %zu COMPOSE WRONG PARAMETER\n", line_num);
      break;
    case WR_AT_VAL:
      fprintf(stderr, "ERROR %zu AT WRONG VALUE\n", line_num);
      break;
    case ST_UND:
      fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", line_num);
      break;
    default:
      break;
  }
}

static bool CanPerformOp(Stack *s, Command command) {
  bool res = true;
  size_t polys_on_stack = StackNumberOfPolys(s);
  switch (command.op) {
    case ZERO: case IS_COEFF: case IS_ZERO: case CLONE: case NEG: case DEG:
    case DEG_BY: case AT: case PRINT: case POP:
      if (StackIsEmpty(s)) {
        PrintError(ST_UND);
        res = false;
      }
      break;
      case ADD: case MUL: case SUB: case IS_EQ:
        if (polys_on_stack < 2) {
          PrintError(ST_UND);
          res = false;
        }
      break;
    case COMPOSE:
      if (polys_on_stack == 0 || polys_on_stack - 1 < command.compose_arg) {
        PrintError(ST_UND);
        res = false;
      }
    default:
      break;
  }
  return res;
}

static void Zero(Stack *s) {
  assert(s);
  StackPush(s, PolyZero());
}

static void IsCoeff(Stack *s, Command command) {
  if (CanPerformOp(s, command)) {
    printf("%d\n", PolyIsCoeff(StackPeekFirst(s)) ? 1 : 0);
  }
}

static void IsZero(Stack *s, Command command) {
  if (CanPerformOp(s, command))
    printf("%d\n", PolyIsZero(StackPeekFirst(s)) ? 1 : 0);
}

static void Clone(Stack *s, Command command) {
  if (CanPerformOp(s, command))
    StackPush(s, PolyClone(StackPeekFirst(s)));
}

static void Add(Stack *s, Command command) {
  if (CanPerformOp(s, command)) {
    Poly p = StackPop(s), q = StackPop(s);
    Poly pq = PolyAdd(&p, &q);
    StackPush(s, PolySimplifyRec(&pq));
    PolyDestroy(&p);
    PolyDestroy(&q);
  }
}

static void Mul(Stack *s, Command command) {
  if (CanPerformOp(s, command)) {
    Poly p = StackPop(s), q = StackPop(s);
    Poly pq = PolyMul(&p, &q);
    StackPush(s, PolySimplifyRec(&pq));
    PolyDestroy(&p);
    PolyDestroy(&q);
  }
}

static void Neg(Stack *s, Command command) {
  if (CanPerformOp(s, command)) {
    Poly p = StackPop(s);
    Poly q = PolyNeg(&p);
    StackPush(s, PolySimplifyRec(&q));
    PolyDestroy(&p);
  }
}

static void Sub(Stack *s, Command command) {
  if (CanPerformOp(s, command)) {
    Poly p = StackPop(s), q = StackPop(s);
    Poly pq = PolySub(&p, &q);
    StackPush(s, PolySimplifyRec(&pq));
    PolyDestroy(&p);
    PolyDestroy(&q);
  }
}

static void IsEq(Stack *s, Command command) {
  if (CanPerformOp(s, command))
    printf("%d\n", PolyIsEq(StackPeekFirst(s), StackPeekSecond(s)) ? 1 : 0);
}

static void Deg(Stack *s, Command command) {
  if (CanPerformOp(s, command))
    printf("%d\n", PolyDeg(StackPeekFirst(s)));
}

static void DegBy(Stack *s, Command command) {
  if (CanPerformOp(s, command))
    printf("%d\n", PolyDegBy(StackPeekFirst(s), command.deg_by_arg));
}

static void At(Stack *s, Command command) {
  if (CanPerformOp(s, command)) {
    Poly p = StackPop(s);
    Poly q = PolyAt(&p, command.at_arg);
    StackPush(s, PolySimplifyRec(&q));
    PolyDestroy(&p);
  }
}

static void Print(Stack *s, Command command) {
  if (CanPerformOp(s, command)) {
    PolyPrint(StackPeekFirst(s));
    printf("\n");
  }
}

static void Pop(Stack *s, Command command) {
  if (CanPerformOp(s, command)) {
    Poly p = StackPop(s);
    PolyDestroy(&p);
  }
}

static void Compose(Stack *s, Command command) {
  if (CanPerformOp(s, command)) {
    Poly p = StackPop(s);
    Poly q[command.compose_arg];
    for (compose_arg_t i = command.compose_arg; i > 0; i--)
      q[i - 1] = StackPop(s);
    Poly composed = PolyCompose(&p, command.compose_arg, q);
    StackPush(s, PolySimplify(&composed));
    PolyDestroy(&p);
    for (size_t i = 0; i < command.compose_arg; i++)
      PolyDestroy(q + i);
  }
}

static void PerformCommand(Stack *s, Command command) {
  switch (command.op) {
    case ZERO:
      Zero(s);
      break;
    case IS_COEFF:
      IsCoeff(s, command);
      break;
    case IS_ZERO:
      IsZero(s, command);
      break;
    case CLONE:
      Clone(s, command);
      break;
    case ADD:
      Add(s, command);
      break;
    case MUL:
      Mul(s, command);
      break;
    case NEG:
      Neg(s, command);
      break;
    case SUB:
      Sub(s, command);
      break;
    case IS_EQ:
      IsEq(s, command);
      break;
    case DEG:
      Deg(s, command);
      break;
    case DEG_BY:
      DegBy(s, command);
      break;
    case AT:
      At(s, command);
      break;
    case COMPOSE:
      Compose(s, command);
      break;
    case PRINT:
      Print(s, command);
      break;
    case POP:
      Pop(s, command);
      break;
    default:
      break;
  }
}

/**
 * Przeprowadza działanie kalkulatora
 * poprzez obsługę poleceń oraz błędów.
 * @return kod wyjścia programu
 */
int main() {
  Line line;
  Stack *s = NewStack();
  do {
    line = GetNextLine();
    LineType type = line.type;
    if (line.error_type != NONE_ERR)
      PrintError(line.error_type);
    else {
      if (type == POLY)
        StackPush(s, line.p);
      else if (type == OPER)
        PerformCommand(s, line.c);
    }
    if (type != EMPTY)
      free(line.chars);
    line_num++;
  } while (!line.is_eof);
  StackFree(s);
  return 0;
}

#endif //POLYNOMIALS_CALC_C