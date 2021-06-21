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
 * @param s : stos
 * @param command : polecenie
 * @return Czy można wykonać operację?
 */
static bool CanPerformOp(Stack *s, Command command);

/**
 * Wstawia na wierzchołek stosu
 * wielomian tożsamościowo równy zeru.
 * @param s : stos
 * @param command : polecenie
 */
static void Zero(Stack *s);

/**
 * Wypisuje na standardowe wyjście 1,
 * jeśli wielomian na wierzchołku stosu
 * jest współczynnikiem - 0 w przeciwnym przypadku.
 * @param s : stos
 * @param command : polecenie
 */
static void IsCoeff(Stack *s, Command command);

/**
 * Wypisuje na standardowe wyjście 1,
 * jeśli wielomian na wierzchołku stosu
 * jest tożsamościowo równy zeru - 0 w przeciwnym
 * przypadku.
 * @param s : stos
 * @param command : polecenie
 */
static void IsZero(Stack *s, Command command);

/**
 * Wstawia na stos kopię
 * wielomianu z wierzhołka.
 * @param s : stos
 * @param command : polecenie
 */
static void Clone(Stack *s, Command command);

/**
 * Dodaje dwa wielomiany z wierzchu stosu,
 * usuwa je i wstawia na wierzchołek
 * stosu ich sumę.
 * @param s : stos
 * @param command : polecenie
 */
static void Add(Stack *s, Command command);

/**
 * Mnoży dwa wielomiany z wierzchu stosu,
 * usuwa je i wstawia na wierzchołek
 * stosu ich iloczyn.
 * @param s : stos
 * @param command : polecenie
 */
static void Mul(Stack *s, Command command);

/**
 * Neguje wielomian na
 * wierzchołku stosu.
 * @param s : stos
 * @param command : polecenie
 */
static void Neg(Stack *s, Command command);

/**
 * Odejmuje od wielomianu z wierzchołka
 * stosu wielomian pod wierzchołkiem,
 * usuwa je i wstawia na wierzchołek stosu
 * ich różnicę.
 * @param s : stos
 * @param command : polecenie
 */
static void Sub(Stack *s, Command command);

/**
 * Wypisuje na standardowe wyjście 1,
 * jeśli dwa wielomiany na wierzchu
 * stosu są równe - 0 w przeciwnym
 * przypadku.
 * @param s : stos
 * @param command : polecenie

 */
static void IsEq(Stack *s, Command command);

/**
 * Wypisuje na standardowe wyjście
 * stopień wielomianu z wierzchołka
 * stosu (-1 dla wielomianu tożsamościowo
 * równego zeru).
 * @param command : polecenie
 */
static void Deg(Stack *s, Command command);

/**
 * Wypisuje na standardowe wyjście stopień
 * wielomianu z wierzchołka stosu ze względu
 * na zmienną o numerze @p var. (-1 dla
 * wielomianu tożsamościowo równego zeru).
 * @param s : stos
 * @param command : polecenie
 */
static void DegBy(Stack *s, Command command);

/**
 * Wylicza wartość wielomianu z wierzchołka
 * stosu w punkcie @p x, usuwa wielomian z
 * wierzchołka i wstawia na stos wynik operacji.
 * @param s : stos
 * @param command : polecenie
 */
static void At(Stack *s, Command command);

/**
 * Wypisuje na standardowe wyjście
 * wielomian z wierzchołka stosu.
 * @param s : stos
 * @param command : polecenie

 */
static void Print(Stack *s, Command command);

/**
 * Usuwa wielomian z
 * wierzchołka stosu.
 * @param s : stos
 * @param command : polecenie
 */
static void Pop(Stack *s, Command command);

/**
 * Zdejmuje z wierzchołka stosu najpierw
 * wielomian p, a potem kolejno wielomiany
 * q[k - 1], q[k - 2], …, q[0] i umieszcza
 * na stosie wynik operacji złożenia.
 * @param s : stos
 * @param command : polecenie
 */
static void Compose(Stack *s, Command command);

/**
 * Wykonuje zadane
 * polecenie na stosie.
 * @param s : stos
 * @param command : polecenie
 */
static void PerformCommand(Stack *s, Command command);

#endif //POLYNOMIALS_CALC_C