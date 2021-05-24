/** @file
  Implementacja kalkulatora działającego na wielomianach rzadkich wielu zmiennych.

  @authors Wojciech Kuzebski <wk429552@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_CALC_H
#define POLYNOMIALS_CALC_C

#include <stddef.h>
#include "parser.h"
#include "stack.h"

/**
 * Wypisuje na standardowe wyjście
 * błędu odpowiedni błąd
 * @param error_type : error
 */
static void PrintError(Error error_type);

/**
 * Sprawdza, czy można wykonać
 * operację na podstawie stanu stosu
 * @param op : operacja
 * @param s : stos
 * @return Czy można wykonać operację?
 */
static bool CanPerformOp(Operator op, Stack *s);

/**
 * Wstawia na wierzchołek stosu
 * wielomian tożsamościowo równy zeru.
 * @param s : stos
 */
static void Zero(Stack *s);

/**
 * Wypisuje na standardowe wyjście 1,
 * jeśli wielomian na wierzchołku stosu
 * jest współczynnikiem - 0 w przeciwnym przypadku.
 * @param s : stos
 */
static void IsCoeff(Stack *s);

/**
 * Wypisuje na standardowe wyjście 1,
 * jeśli wielomian na wierzchołku stosu
 * jest tożsamościowo równy zeru - 0 w przeciwnym
 * przypadku.
 * @param s : stos
 */
static void IsZero(Stack *s);

/**
 * Wstawia na stos kopię
 * wielomianu z wierzhołka.
 * @param s : stos
 */
static void Clone(Stack *s);

/**
 * Dodaje dwa wielomiany z wierzchu stosu,
 * usuwa je i wstawia na wierzchołek
 * stosu ich sumę.
 * @param s : stos
 */
static void Add(Stack *s);

/**
 * Mnoży dwa wielomiany z wierzchu stosu,
 * usuwa je i wstawia na wierzchołek
 * stosu ich iloczyn.
 * @param s : stos
 */
static void Mul(Stack *s);

/**
 * Neguje wielomian na
 * wierzchołku stosu.
 * @param s : stos
 */
static void Neg(Stack *s);

/**
 * Odejmuje od wielomianu z wierzchołka
 * stosu wielomian pod wierzchołkiem,
 * usuwa je i wstawia na wierzchołek stosu
 * ich różnicę.
 * @param s : stos
 */
static void Sub(Stack *s);

/**
 * Wypisuje na standardowe wyjście 1,
 * jeśli dwa wielomiany na wierzchu
 * stosu są równe - 0 w przeciwnym
 * przypadku.
 * @param s : stos
 */
static void IsEq(Stack *s);

/**
 * Wypisuje na standardowe wyjście
 * stopień wielomianu z wierzchołka
 * stosu (-1 dla wielomianu tożsamościowo
 * równego zeru).
 * @param s : stos
 */
static void Deg(Stack *s);

/**
 * Wypisuje na standardowe wyjście stopień
 * wielomianu z wierzchołka stosu ze względu
 * na zmienną o numerze @p var. (-1 dla
 * wielomianu tożsamościowo równego zeru).
 * @param s : stos
 * @param var : zmienna
 */
static void DegBy(Stack *s, deg_by_arg_t var);

/**
 * Wylicza wartość wielomianu z wierzchołka
 * stosu w punkcie @p x, usuwa wielomian z
 * wierzchołka i wstawia na stos wynik operacji.
 * @param s : stos
 * @param x : punkt
 */
static void At(Stack *s, at_arg_t x);

/**
 * Wypisuje na standardowe wyjście
 * wielomian z wierzchołka stosu.
 * @param s : stos
 */
static void Print(Stack *s);

/**
 * Usuwa wielomian z
 * wierzchołka stosu.
 * @param s : stos
 */
static void Pop(Stack *s);

/**
 * Wykonuje zadane
 * polecenie na stosie.
 * @param s : stos
 * @param command : polecenie
 */
static void PerformCommand(Stack *s, Command command);

#endif //POLYNOMIALS_CALC_C