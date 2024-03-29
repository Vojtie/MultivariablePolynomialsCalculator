/** @file
 * Implementacja interfejsu stosu przechowującego wielomiany.
 *
 * @authors Wojciech Kuzebski <wk429552@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2021
*/

#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"

/** To jest stała przechowująca domyślny rozmiar stosu */
#define DEF_STACK_SIZE 32

/** To jest stała przechowująca domyślny współczynnik zwiększania rozmiaru stosu */
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
  res->top = 0;
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
  return s->top == 0;
}

Poly StackPop(Stack *s) {
  assert(s && !StackIsEmpty(s));
  return s->polys[--s->top];
}

void StackPush(Stack *s, Poly p) {
  assert(s && s->size > 0);
  if (StackIsFull(s))
    StackIncrease(s);
  s->polys[s->top++] = p;
}

Poly *StackPeekFirst(Stack *s) {
  assert(s && !StackIsEmpty(s));
  return &s->polys[s->top - 1];
}

Poly *StackPeekSecond(Stack *s) {
  assert(s && !StackIsEmpty(s));
  return &s->polys[s->top - 2];
}

size_t StackNumberOfPolys(Stack *s) {
  assert(s);
  return s->top;
}

void StackFree(Stack *s) {
  while (!StackIsEmpty(s)) {
    Poly p = StackPop(s);
    PolyDestroy(&p);
  }
  free(s->polys);
  free(s);
}
