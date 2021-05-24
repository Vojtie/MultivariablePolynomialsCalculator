/** @file
 * Implementacja interfejsu stosu przechowującego wielomiany.
 *
 * @authors Wojciech Kuzebski <wk429552@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2021
*/

#ifndef POLYNOMIALS_STACK_C
#define POLYNOMIALS_STACK_C

#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"

#define DEF_STACK_SIZE 32
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

Stack *NewStack() {
  Stack *res = malloc(sizeof *res);
  CheckAlloc(res);
  res->polys = malloc(DEF_STACK_SIZE * sizeof *res->polys);
  CheckAlloc(res->polys);
  res->top = -1;
  res->size = DEF_STACK_SIZE;
  return res;
}

/**
 * Zwiększa rozmiar pamięci
 * zaalokowanej dla stosu.
 * @param s : stos
 */
static void StackIncrease(Stack *s) {
  assert(s);
  s->size *= DEF_ALLOC_COEFF;
  s->polys = realloc(s->polys, s->size * sizeof(*s->polys));
  CheckAlloc(s->polys);
}

/**
 * Sprawdza czy stos jest pełny.
 * @param s : stos
 * @return Czy stos jest pełny?
 */
static bool StackIsFull(Stack *s) {
  assert(s);
  return s->top == s->size - 1;
}

bool StackIsEmpty(Stack *s) {
  assert(s);
  return s->top == -1;
}

Poly StackPop(Stack *s) {
  assert(s && !StackIsEmpty(s));
  return s->polys[s->top--];
}

void StackPush(Stack *s, Poly p) {
  assert(s && s->size > 0);
  if (StackIsFull(s))
    StackIncrease(s);
  s->polys[++s->top] = p;
}

Poly *StackPeekFirst(Stack *s) {
  assert(s && !StackIsEmpty(s));
  return &s->polys[s->top];
}

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

#endif //POLYNOMIALS_STACK_C
