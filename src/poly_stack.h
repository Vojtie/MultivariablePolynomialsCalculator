#ifndef POLYNOMIALS_POLY_STACK_H
#define POLYNOMIALS_POLY_STACK_H

typedef struct Stack {
    Poly *polys;
    long top;
    long size;
} Stack;


Stack *NewStack();

Poly *PeekFirst(Stack *s);

Poly *PeekSecond(Stack *s);

bool IsFull(Stack *s);

static Poly *StackPop(Stack *s);

bool IsEmpty(Stack *s);

void Push(Stack *s, Poly p);

size_t GetNumberOfPolys(Stack *s);

void StackFree(Stack *s);

#endif //POLYNOMIALS_POLY_STACK_H
