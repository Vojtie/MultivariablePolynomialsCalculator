#include "poly_parser.h"
#include "poly.h"
#include "poly_stack.c"
#include <stdio.h>

/**
 * czy powinniśmy wszędzie dodać assert(ptr)
 * czy line_num może być globalne
 *
 */
static size_t line_num = 1;

static bool CanPerformOp(Operator op, Stack *s) {
  bool res = true;
  switch (op) {
    case ZERO: case IS_COEFF: case IS_ZERO: case CLONE: case NEG: case DEG:
    case DEG_BY: case AT: case PRINT: case POP:
      if (IsEmpty(s)) {
        PrintError(line_num, ST_UND);
        res = false;
      }
      break;
      case ADD: case MUL: case SUB: case IS_EQ:
        if (GetNumberOfPolys(s) < 2) {
          PrintError(line_num, ST_UND);
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
  Push(s, PolyZero());
}

static void IsCoeff(Stack *s) {
  if (CanPerformOp(IS_COEFF, s))
    printf("%d\n", PolyIsCoeff(PeekFirst(s)) ? 1 : 0);
}

static void IsZero(Stack *s) {
  if (CanPerformOp(IS_ZERO, s))
    printf("%d\n", PolyIsZero(PeekFirst(s)) ? 1 : 0);
}

static void Clone(Stack *s) {
  if (CanPerformOp(CLONE, s))
    Push(s, PolyClone(PeekFirst(s)));
}

static void Add(Stack *s) {
  if (CanPerformOp(ADD, s))
    Push(s, PolyAdd(StackPop(s), StackPop(s)));
}

static void Mul(Stack *s) {
  if (CanPerformOp(MUL, s))
    Push(s, PolyMul(StackPop(s), StackPop(s)));
}

static void Neg(Stack *s) {
  if (CanPerformOp(NEG, s))
    Push(s, PolyNeg(StackPop(s)));
}

static void Sub(Stack *s) {
  if (CanPerformOp(SUB, s))
    Push(s, PolySub(StackPop(s), StackPop(s)));
}

static void IsEq(Stack *s) {
  if (CanPerformOp(IS_EQ, s))
    printf("%d\n", PolyIsEq(PeekFirst(s), PeekSecond(s)) ? 1 : 0);
}

static void Deg(Stack *s) {
  if (CanPerformOp(DEG, s))
    printf("%d\n", PolyDeg(PeekFirst(s)));
}

static void DegBy(Stack *s, deg_by_arg_t var) {
  if (CanPerformOp(DEG_BY, s))
    printf("%d\n", PolyDegBy(PeekFirst(s), var));
}

static void At(Stack *s, at_arg_t var) {
  if (CanPerformOp(AT, s))
    Push(s, PolyAt(StackPop(s), var));
}

static void Print(Stack *s) {
  if (CanPerformOp(PRINT, s)) {
    PolyPrint(PeekFirst(s));
    printf("\n");
  }
}

static void Pop(Stack *s) {
  if (!IsEmpty(s))
    StackDelTop(s);
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
         PrintError(line_num, line.error_type);
     }
     else if (line.type == OPER_LINE) {
       Command command = ReadCommand(&line);
       if (line.is_correct)
         PerformCommand(s, command);
       else
         PrintError(line_num, line.error_type);
     }
     free(string);
   }
   else PrintError(line_num, line.error_type);
   line_num++;
 }
 while (!line.is_eof);
 StackFree(s);
 return (0);
}