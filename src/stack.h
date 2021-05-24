/** @file
 * Interfejs stosu przechowującego wielomiany.
 *
 * @authors Wojciech Kuzebski <wk429552@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2021
 */

#ifndef POLYNOMIALS_STACK_H
#define POLYNOMIALS_STACK_H

#include <stddef.h>
#include "poly.h"

/**
 * Struktura implementująca stos
 * przechowujący wielomiany.
 */
typedef struct Stack {
    Poly *polys;
    long top;
    long size;
} Stack;

/**
 * Zwraca wskaźnik na nowy stos.
 * @return stos
 */
Stack *NewStack();

/**
 * Zwraca wskaźnik na wielomian
 * z wierzchołka stosu.
 * @param s : stos
 * @return wielomian
 */
Poly *StackPeekFirst(Stack *s);

/**
 * Zwraca wskaźnik na wielomian
 * spod wierzchołka stosu.
 * @param s : stos
 * @return wielomian
 */
Poly *StackPeekSecond(Stack *s);

/**
 * Zwraca i usuwa ze stosu
 * wielomian z wierzchołka stosu.
 * @param s : stos
 * @return wielomian
 */
Poly StackPop(Stack *s);

/**
 * Sprawdza, czy stos jest pusty.
 * @param s : stos
 * @return Czy stos jest pusty?
 */
bool StackIsEmpty(Stack *s);

/**
 * Wstawia wielomian @p p
 * na wierzchołek stosu.
 * @param s : stos
 * @param p : wielomian
 */
void StackPush(Stack *s, Poly p);

/**
 * Zwraca liczbę przechowywanych
 * na stosie wielomianów.
 * @param s : stos
 * @return liczba wielomianów
 */
size_t StackNumberOfPolys(Stack *s);

/**
 * Zwalnia pamięć zaalokowaną
 * na stos.
 * @param s : stos
 */
void StackFree(Stack *s);

#endif //POLYNOMIALS_STACK_H
