#ifndef POLYNOMIALS_POLY_STACK_H
#define POLYNOMIALS_POLY_STACK_H

typedef struct Stack {
    Poly *polys;
    long top;
    long size;
} Stack;

Stack *NewStack();

Poly *StackPeekFirst(Stack *s);

Poly *StackPeekSecond(Stack *s);

Poly StackPop(Stack *s);

bool StackIsEmpty(Stack *s);

void StackPush(Stack *s, Poly p);

size_t StackNumberOfPolys(Stack *s);

void StackFree(Stack *s);

#endif //POLYNOMIALS_POLY_STACK_H
