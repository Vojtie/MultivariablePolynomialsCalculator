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

static void InreaseStack(Stack *s) {
  assert(s);
  s->polys = realloc(s->polys, s->size * DEF_ALLOC_COEFF);
  CheckAlloc(s->polys);
  s->size *= DEF_ALLOC_COEFF;
}

bool IsFull(Stack *s) {
  assert(s);
  return s->top == s->size - 1;
}

bool IsEmpty(Stack *s) {
  assert(s);
  return /*s == NULL || */s->top == -1;
}

Poly *StackPop(Stack *s) {
  assert(s && !IsEmpty(s));
  return  &s->polys[s->top--];
}

void Push(Stack *s, Poly p) {
  assert(s && s->size > 0);
  if (IsFull(s))
    InreaseStack(s);
  s->polys[++s->top] = p;
}

Poly *PeekFirst(Stack *s) {
  assert(s && !IsEmpty(s));
  return &s->polys[s->top];
}

Poly *PeekSecond(Stack *s) {
  assert(s && !IsEmpty(s));
  return &s->polys[s->top - 1];
}

size_t GetNumberOfPolys(Stack *s) {
  assert(s);
  return s->top + 1;
}

void StackDelTop(Stack *s) {
  assert(s);
  PolyDestroy(StackPop(s));
}
// uzupełnić
void StackFree(Stack *s) {
  free(s->polys);
  free(s);
}

#endif //POLYNOMIALS_POLY_STACK_C
