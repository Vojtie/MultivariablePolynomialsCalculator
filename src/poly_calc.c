#include "poly_parser.h"
#include "poly_stack.h"
#include <stdio.h>

/**
 * czy powinniśmy wszędzie dodać assert(ptr)
 * czy line_num może być globalne
 *
 */
static size_t line_num = 1;

void PrintError(Error error_type) {
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

static bool CanPerformOp(Operator op, Stack *s) {
  bool res = true;
  switch (op) {
    case ZERO: case IS_COEFF: case IS_ZERO: case CLONE: case NEG: case DEG:
    case DEG_BY: case AT: case PRINT: case POP:
      if (StackIsEmpty(s)) {
        PrintError(ST_UND);
        res = false;
      }
      break;
      case ADD: case MUL: case SUB: case IS_EQ:
        if (StackNumberOfPolys(s) < 2) {
          PrintError(ST_UND);
          res = false;
        }
      break;
    default:
      break;
  }
  return res;
}

static void Zero(Stack *s) {
  assert(s);
  StackPush(s, PolyZero());
}

static void IsCoeff(Stack *s) {
  if (CanPerformOp(IS_COEFF, s)) {
    printf("%d\n", PolyIsCoeff(StackPeekFirst(s)) ? 1 : 0);
  }
}

static void IsZero(Stack *s) {
  if (CanPerformOp(IS_ZERO, s))
    printf("%d\n", PolyIsZero(StackPeekFirst(s)) ? 1 : 0);
}

static void Clone(Stack *s) {
  if (CanPerformOp(CLONE, s))
    StackPush(s, PolyClone(StackPeekFirst(s)));
}

static void Add(Stack *s) {
  if (CanPerformOp(ADD, s)) {
    Poly p = StackPop(s), q = StackPop(s);
    StackPush(s, PolyAdd(&p, &q));
    PolyDestroy(&p);
    PolyDestroy(&q);
  }
}

static void Mul(Stack *s) {
  if (CanPerformOp(MUL, s)) {
    Poly p = StackPop(s), q = StackPop(s);
    StackPush(s, PolyMul(&p, &q));
    PolyDestroy(&p);
    PolyDestroy(&q);
  }
}

static void Neg(Stack *s) {
  if (CanPerformOp(NEG, s)) {
    Poly p = StackPop(s);
    StackPush(s, PolyNeg(&p));
    PolyDestroy(&p);
  }
}

static void Sub(Stack *s) {
  if (CanPerformOp(SUB, s)) {
    Poly p = StackPop(s), q = StackPop(s);
    StackPush(s, PolySub(&p, &q));
    PolyDestroy(&p);
    PolyDestroy(&q);
  }
}

static void IsEq(Stack *s) {
  if (CanPerformOp(IS_EQ, s))
    printf("%d\n", PolyIsEq(StackPeekFirst(s), StackPeekSecond(s)) ? 1 : 0);
}

static void Deg(Stack *s) {
  if (CanPerformOp(DEG, s))
    printf("%d\n", PolyDeg(StackPeekFirst(s)));
}

static void DegBy(Stack *s, deg_by_arg_t var) {
  if (CanPerformOp(DEG_BY, s))
    printf("%d\n", PolyDegBy(StackPeekFirst(s), var));
}

static void At(Stack *s, at_arg_t var) {
  if (CanPerformOp(AT, s)) {
    Poly p = StackPop(s);
    StackPush(s, PolyAt(&p, var));
    PolyDestroy(&p);
  }
}

static void Print(Stack *s) {
  if (CanPerformOp(PRINT, s)) {
    PolyPrint(StackPeekFirst(s));
    printf("\n");
  }
}

static void Pop(Stack *s) {
  if (CanPerformOp(POP, s)) {
    Poly p = StackPop(s);
    PolyDestroy(&p);
  }
}

static void PerformCommand(Stack *s, Command command) {
  switch (command.op) {
    case ZERO:
      Zero(s);
      break;
    case IS_COEFF:
      IsCoeff(s);
      break;
    case IS_ZERO:
      IsZero(s);
      break;
    case CLONE:
      Clone(s);
      break;
    case ADD:
      Add(s);
      break;
    case MUL:
      Mul(s);
      break;
    case NEG:
      Neg(s);
      break;
    case SUB:
      Sub(s);
      break;
    case IS_EQ:
      IsEq(s);
      break;
    case DEG:
      Deg(s);
      break;
    case DEG_BY:
      DegBy(s, command.deg_by_arg);
      break;
    case AT:
      At(s, command.at_arg);
      break;
    case PRINT:
      Print(s);
      break;
    case POP:
      Pop(s);
      break;
    default:
      break;
  }
}

int main() {
  Line line;
  Error err;
  Stack *s = NewStack();
  do {
    line = GetNextLine();
    if ((err = line.error_type) != NONE_ERR)
      PrintError(err);
    else {
      LineType type = line.type;
      if (type == POLY)
        StackPush(s, line.p);
      else if (type == OPER)
        PerformCommand(s, line.c);
      if (type != EMPTY)
        free(line.chars);
    }
    line_num++;
  }
  while (!line.is_eof);
  StackFree(s);
}

/**
int main() {
  Line line;
  Stack *s = NewStack();
 do {
   line = ReadLine();
   if (line.is_correct) {
     char *string = line.chars;
     if (line.type == POLY_LINE) {
       CheckLimits(&line);
       if (line.is_correct)
         Push(s, StrToPoly(&line.chars));
       else
         PrintError(line.error_type);
     }
     else if (line.type == OPER_LINE) {
       Command command = ReadCommand(&line);
       if (line.is_correct)
         PerformCommand(s, command);
       else
         PrintError(line.error_type);
     }
     free(string);
   }
   else PrintError(line.error_type);
   line_num++;
 }
 while (!line.is_eof);
 StackFree(s);
 return (0);
}
 */