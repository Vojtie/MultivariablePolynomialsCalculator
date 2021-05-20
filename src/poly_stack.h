#ifndef POLYNOMIALS_POLY_STACK_H
#define POLYNOMIALS_POLY_STACK_H

#include <stdlib.h>

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
typedef struct Stack {
    Poly *polys;
    long top;
    long size;
} Stack;

static Stack *NewStack(size_t size) {
  assert(size > 0);
  Stack *res = malloc(sizeof *res);
  CheckAlloc(res);
  res->polys = malloc(size * sizeof *res->polys);
  CheckAlloc(res->polys);
  res->top = 0;
  res->size = size;
  return res;
}

static void InreaseStack(Stack *s) {
  assert(s);
  s->polys = realloc(s->polys, s->size * DEF_ALLOC_COEFF);
  CheckAlloc(s->polys);
  s->size *= DEF_ALLOC_COEFF;
}

static bool IsFull(Stack *s) {
  assert(s);
  return s->top == s->size - 1;
}

static bool IsEmpty(Stack *s) {
  assert(s);
  return /*s == NULL || */s->top == -1;
}

static Poly Pop(Stack *s) {
  assert(s);
  Poly res = NULL;
  if (!IsEmpty(s))
    res = s->polys[s->top--];
  return res;
}

static void Push(Stack *s, Poly p) {
  assert(s && s->size > 0);
  if (IsFull(s))
    InreaseStack(s);
  s->polys[++s->top] = p;
}

static Poly Peek(Stack *s) {
  assert(s && !IsEmpty(s));
  return s->polys[s->top];
}
























































#endif //POLYNOMIALS_POLY_STACK_H
