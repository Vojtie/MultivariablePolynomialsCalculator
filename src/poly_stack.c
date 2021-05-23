#ifndef POLYNOMIALS_POLY_STACK_C
#define POLYNOMIALS_POLY_STACK_C

#include <stdlib.h>
#include "poly_stack.h"
#include "stdbool.h"

#define DEF_STACK_SIZE 10
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
 * założenia:
 * nie tworzę pustych stosów
 *
 */

Stack *NewStack() {
  Stack *res = malloc(sizeof *res);
  CheckAlloc(res);
  res->polys = malloc(DEF_STACK_SIZE * sizeof *res->polys);
  CheckAlloc(res->polys);
  res->top = -1;
  res->size = DEF_STACK_SIZE;
  return res;
}

static void StackInrease(Stack *s) {
  assert(s);
  s->polys = realloc(s->polys, s->size * DEF_ALLOC_COEFF);
  CheckAlloc(s->polys);
  s->size *= DEF_ALLOC_COEFF;
}

static bool StackIsFull(Stack *s) {
  assert(s);
  return s->top == s->size - 1;
}

bool StackIsEmpty(Stack *s) {
  assert(s);
  return /*s == NULL || */s->top == -1;
}

Poly StackPop(Stack *s) {
  assert(s && !StackIsEmpty(s));
  return s->polys[s->top--];
}

void StackPush(Stack *s, Poly p) {
  assert(s && s->size > 0);
  if (StackIsFull(s))
    StackInrease(s);
  s->polys[++s->top] = p;
}

Poly *StackPeekFirst(Stack *s) {
  assert(s && !StackIsEmpty(s));
  return &s->polys[s->top];
}
/**
 *
static bool IsCorrectPolyChar(int last_ch, int ch) {
  bool res = true;
  if (ch != '(' && ch != ')' && ch != ',' && ch != '-' && ch != '+' && !IsNumber(ch))
    res = false;
  else if (last_ch == '(' && (ch == ',' || ch == ')' || ch == '+'))//ch != '(' && !IsNumber(ch) && ch != '-'))
    res = false;
  else if (last_ch == ',' && (ch == '(' || ch == ')' || ch == '+'))// !IsNumber(ch) && ch != '-')
    res = false;
  else if (last_ch == ')' && (ch == ''))//ch != ',' && ch != '+')
    res = false;
  else if (last_ch == '-' && !IsNumber(ch))
    res = false;
  else if (IsNumber(last_ch) && !IsNumber(ch) && ch != ',' && ch != ')')
    res = false;
  else if (last_ch == '+' && ch != '(')
    res = false;
  return res;
}
 */

Poly *StackPeekSecond(Stack *s) {
  assert(s && !StackIsEmpty(s));
  return &s->polys[s->top - 1];
}

size_t StackNumberOfPolys(Stack *s) {
  assert(s);
  return s->top + 1;
}

void StackFree(Stack *s) {
  while (!StackIsEmpty(s)) {
    Poly p = StackPop(s);
    PolyDestroy(&p);
  }
  free(s->polys);
  free(s);
}

#endif //POLYNOMIALS_POLY_STACK_C
