/** @file
  Implementacja klasy wielomianów rzadkich wielu zmiennych

  @authors Wojciech Kuzebski <wk429552@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <stdlib.h>
#include <stdio.h>
#include "poly.h"

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
 * Porównuje dwa wykładniki.
 * @param[in] a : wykładnik
 * @param[in] b : wykładnik
 * @return większy wykładnik
 */
static poly_exp_t ExpMax(poly_exp_t a, poly_exp_t b) {
  if (a > b)
    return a;
  else
    return b;
}

/**
 * Zwraca rozmiar wielomianu,
 * który nie jest współczynnikiem.
 * @param[in] p : wielomian
 * @return rozmiar wielomianu.
 */
static size_t PolyGetSize(const Poly *p) {
  assert(p);
  return p->size;
}

/**
 * Zwraca współczynnik wielomianu,
 * który jest współczynnikiem.
 * @param[in] p : wielomian
 * @return wartość współczynnika wielomianu
 */
static poly_coeff_t PolyGetCoeff(const Poly *p) {
  assert(p && PolyIsCoeff(p));
  return p->coeff;
}

/**
 * Zwraca tablicę przechowującą
 * listę jednomianów wielomianu.
 * @param[in] p : wielomian
 * @return tablica przechowująca listę jednomianów
 */
static Mono *PolyGetArr(const Poly *p) {
  assert(p);
  return p->arr;
}

/**
 * Zwraca wielomian, który jest
 * współczynnikiem jednomianu.
 * @param[in] m : jednomian
 * @return wielomian
 */
static Poly *MonoGetPtrToPoly(const Mono *m) {
  return (Poly *)&m->p;
}

void PolyDestroy(Poly *p) {
  assert(p);
  if (!PolyIsCoeff(p)) {
    for (size_t i = 0; i < p->size; i++)
      MonoDestroy(&p->arr[i]);
    free(PolyGetArr(p));
  }
}

/**
 * Alokuje pamięć dla tablicy jednomianów.
 * @param count : liczba jednomianów
 * @return tablica jednomianów
 */
static Mono *AllocMemForMonos(size_t count) {
  Mono *res = calloc(count, sizeof(*res));
  CheckAlloc(res);
  return res;
}

/**
 * Alokuje pamięć dla tablicy zmiennych typu logicznego.
 * @param count : liczba elementów tablicy
 * @return tablica typu bool
 */
static bool *AllocMemForBools(size_t count) {
  bool *res = calloc(count, sizeof(*res));
  CheckAlloc(res);
  return res;  
}

Poly PolyClone(const Poly *p) {
  assert(p);
  Poly copy;
  if (PolyIsCoeff(p)) {
    copy.arr = NULL;
    copy.coeff = p->coeff;
  } else {
    copy.size = PolyGetSize(p);
    copy.arr = AllocMemForMonos(PolyGetSize(&copy));
    for (size_t i = 0; i < PolyGetSize(&copy); i++) {
      copy.arr[i] = MonoClone(&p->arr[i]);
    }
  }
  return copy;
}

/**
 * Zwraca wielomian składający się z tablicy jednomianów
 * wielomianu @p p oraz jednomianu o wykładniku równym
 * zeru i współczynniku, będącym wielomianem stałym o
 * współczynniku @p coeff.
 * @param[in] p : wielomian
 * @param[in] coeff : współczynnik
 * @return wielomian
 */
static Poly PolyAddCoeffToMonos(const Poly *p, poly_coeff_t coeff) {
  assert(p && !PolyIsCoeff(p));
  Poly res;
  if (coeff == 0) {
    res = PolyClone(p);
  } else {
    res.size = PolyGetSize(p) + 1;
    res.arr = AllocMemForMonos(PolyGetSize(&res));
    for (size_t i = 0; i < res.size - 1; i++) // zmiana z PolyGetSize(p)
      res.arr[i] = MonoClone(&p->arr[i]);
    res.arr[PolyGetSize(p)].p = PolyFromCoeff(coeff);
    res.arr[PolyGetSize(p)].exp = 0;
    /*
    PolyPrint(&res);
    printf("\n"); */
  }
  return res;
}

/**
 * Zwraca wielomian @f$p+q@f$, którego tablica jednomianów
 * składa się z kopii tablic jednomianów wielomianów
 * @p p i @p q.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return wielomian @f$p+q@f$.
 */
static Poly PolyMergeTwoPolys(const Poly *p, const Poly *q) {
  assert(p && q);
  Poly res;
  res.size = PolyGetSize(p) + PolyGetSize(q);
  res.arr = AllocMemForMonos(PolyGetSize(&res));
  size_t p_size = PolyGetSize(p);
  for (size_t i = 0; i < p_size; i++) {
    res.arr[i] = MonoClone(&p->arr[i]);
  }
  for (size_t i = p_size, j = 0; i < res.size; i++, j++) {
    res.arr[i] = MonoClone(&q->arr[j]);
  }
  /*
  PolyPrint(&res);
  printf("\n");
   */
  return res;
}

/**
 * Zwraca wielomian, który jest kopią wielomianu
 * @p p z pominięcięm jednomianów zerowych.
 * Przejmuje na własność wielomian @p p.
 * @param[in] p : wielomian
 * @return wielomian nie zawierający jednomianów zerowych.
 */
Poly PolyDelZeros(Poly *p) {
  assert(p && !PolyIsCoeff(p));
  Poly res;
  bool *zero = AllocMemForBools(PolyGetSize(p));
  size_t count = 0;
  for (size_t i = 0; i < PolyGetSize(p); i++) {
    if (PolyIsZero(&p->arr[i].p)) {
      count++;
      zero[i] = true;
    }
  }
  if (count == 0) {
    res.arr = PolyGetArr(p);
    res.size = PolyGetSize(p);
  } else if (count == PolyGetSize(p)) {
    res = PolyZero();
    PolyDestroy(p);
  } else {
    res.size = PolyGetSize(p) - count;
    res.arr = AllocMemForMonos(PolyGetSize(&res));
    for (size_t i = 0, j = 0; i < PolyGetSize(p); i++) {
      if (!zero[i]) {
        res.arr[j] = p->arr[i];
        j++;
      }
    }
    free(PolyGetArr(p));
  }
  free(zero);
  return res;  
}

Poly PolyAdd(const Poly *p, const Poly *q) {
  assert(p && q);
  Poly res;
  if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
    res = PolyFromCoeff(PolyGetCoeff(p) + PolyGetCoeff(q));
  } else if (PolyIsCoeff(p)) {
    res = PolyAddCoeffToMonos(q, PolyGetCoeff(p));
  } else if (PolyIsCoeff(q)) {
    res = PolyAddCoeffToMonos(p, PolyGetCoeff(q));
  } else {
    res = PolyMergeTwoPolys(p, q);
  }
  if (!PolyIsCoeff(&res)) {
    Mono *temp = PolyGetArr(&res);
    res = PolyAddMonos(res.size, res.arr);
    free(temp);
  }
  /*
  PolyPrint(&res);
  printf("\n");
   */
  return PolySimplify(&res);
}

/**
 * Porównuje wykładniki dwóch jednomianów
 * zwracając ich różnicę.
 * @param[in] ptr_to_mono_1 : jednomian
 * @param[in] ptr_to_mono_2 : jednomian
 * @return różnica wykładników
 */
static poly_exp_t CompareMonos(const void *ptr_to_mono_1, const void *ptr_to_mono_2) {
  assert(ptr_to_mono_1 && ptr_to_mono_2);
  const Mono *ptr1 = (Mono *)ptr_to_mono_1;
  const Mono *ptr2 = (Mono *)ptr_to_mono_2;
  return MonoGetExp(ptr1) - MonoGetExp(ptr2);
}

/**
 * Sortuje tablicę jednomianów wielomianu @p p
 * niemalejąco ze względu na wykładniki.
 * @param[in] p : wielomian
 */
static void SortPoly(const Poly *p);

/**
 * Sortuje tablicę jednomianów niemalejąco
 * ze względu na wykładniki.
 * @param[in] monos : tablica jednomianów
 * @param[in] count : liczba jednomianów
 */
static void SortMonos(Mono monos[], size_t count) {
  assert(monos);
  qsort(monos, count, sizeof(Mono), CompareMonos);
  for (size_t i = 0; i < count; i++)
    SortPoly(&monos[i].p);
}

static void SortPoly(const Poly *p) {
  assert(p);
  if (!PolyIsCoeff(p))
    SortMonos(PolyGetArr(p), PolyGetSize(p));
}

/**
 * Zwraca wielomian, którego tablica jednomianów
 * stanowi kopię tablicy jednomianów @p monos.
 * Przejmuje na własnosć zawartość tablicy @p monos.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian o skopiowanej tablicy jednomianów
 */
static Poly PolyShallowCopy(size_t count, const Mono monos[]) {
  assert(monos);
  Mono *copy = AllocMemForMonos(count);
  for (size_t i = 0; i < count; i++) {
    copy[i].exp = MonoGetExp(&monos[i]);
    copy[i].p.arr = PolyGetArr(&monos[i].p);
    copy[i].p.size = PolyGetSize(&monos[i].p);
  } 
  return PolyFromMonos(copy, count);
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
  assert(monos);
  if (count == 0)
    return PolyZero();
  else {
    Poly copy = PolyShallowCopy(count, monos), temp1, temp2;
    SortPoly(&copy);
    for (size_t i = 0; i < PolyGetSize(&copy); i++) {
      if (PolyIsZero(&copy.arr[i].p))
        continue;
      else {
        for (size_t j = i + 1; j < PolyGetSize(&copy)
          && MonoGetExp(&copy.arr[i]) == MonoGetExp(&copy.arr[j]); j++) {
          temp1 = copy.arr[i].p;
          temp2 = copy.arr[j].p;
          copy.arr[i].p = PolyAdd(&temp1, &temp2);
          copy.arr[j].p = PolyZero();
          PolyDestroy(&temp1);
          PolyDestroy(&temp2);
        }
      }
    }
    return PolyDelZeros(&copy);
  }
}

/**
 * Sprawdza równość posortowanych tablic
 * jednomianów wielomianów @p p i @p q.
 * @param p_monos : tablica jednomianów
 * @param p_size : liczba jednomianów
 * @param q_monos : tablica jednomianów
 * @param q_size : liczba jednomianów
 * @return @f$p = q@f$
 */
static bool MonosAreEq(Mono p_monos[], size_t p_size, Mono q_monos[], size_t q_size) {
  assert(p_monos && q_monos);
  size_t p_i, q_i;
  for (p_i = 0, q_i = 0; p_i < p_size && q_size > q_i; p_i++, q_i++) {
    assert(p_i == 0 || MonoGetExp(&p_monos[p_i]) >= MonoGetExp(&q_monos[p_i - 1]));
    assert(q_i == 0 || MonoGetExp(&q_monos[q_i]) >= MonoGetExp(&q_monos[q_i - 1]));
    while (p_i < p_size && PolyIsZero(MonoGetPtrToPoly(&p_monos[p_i])))
      p_i++;
    while (q_i < q_size && PolyIsZero(MonoGetPtrToPoly(&q_monos[q_i])))
      q_i++;
    if (p_i < p_size && q_size > q_i && ((MonoGetExp(&p_monos[p_i]) != MonoGetExp(&q_monos[q_i]))
        || (!PolyIsEq(MonoGetPtrToPoly(&p_monos[p_i]), MonoGetPtrToPoly(&q_monos[q_i])))))
      return false;
  }
  while (p_i < p_size) {
    if (!PolyIsZero(MonoGetPtrToPoly(&p_monos[p_i])))
      return false;
    p_i++;
  }
  while (q_i < q_size) {
    if (!PolyIsZero(MonoGetPtrToPoly(&q_monos[q_i])))
      return false;
    q_i++;
  }
  return true;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
  assert(p && q);
  bool res = false;
  if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
    res = PolyGetCoeff(p) == PolyGetCoeff(q);
  } else if ((!PolyIsCoeff(p) && !PolyIsCoeff(q))) {
    Poly p_clone = PolyClone(p), q_clone = PolyClone(q);
    SortPoly(p);
    SortPoly(q);
    res = MonosAreEq(PolyGetArr(&p_clone), PolyGetSize(&p_clone), PolyGetArr(&q_clone), PolyGetSize(&q_clone));
    PolyDestroy(&p_clone);
    PolyDestroy(&q_clone);
  } else if (!PolyIsCoeff(p) && PolyIsCoeff(q)) {
    Mono q_mono = MonoFromPoly(q, 0);
    Poly p_clone = PolyClone(p);
    SortPoly(p);
    res = MonosAreEq(PolyGetArr(&p_clone), PolyGetSize(&p_clone), &q_mono, 1);
    PolyDestroy(&p_clone);
  } else if (PolyIsCoeff(p) && !PolyIsCoeff(q)) {
    Mono p_mono = MonoFromPoly(p, 0);
    Poly q_clone = PolyClone(q);
    SortPoly(q);
    res = MonosAreEq(PolyGetArr(&q_clone), PolyGetSize(&q_clone), &p_mono, 1);
    PolyDestroy(&q_clone);
  }
  return res;
}

/**
 * Zwraca przeciwny współczynnik.
 * @param coeff : współczynnik wielomianu
 * @return przeciwny współczynnik.
 */
static poly_coeff_t NegCoeff(poly_coeff_t coeff) {
  return -(coeff);
}

/**
 * Zwraca wielomian o przeciwnych jednomianach.
 * @param count : liczba jednomianów
 * @param monos : tablica jednomianów
 * @return wielomian o przeciwnych jednomianach.
 */
static Poly PolyNegMonos(size_t count, const Mono monos[]) {
  assert(monos);
  Mono *res = AllocMemForMonos(count);
  for (size_t i = 0; i < count; i++) {
    if (PolyIsCoeff(&monos[i].p))
      res[i].p = PolyNeg(&monos[i].p);
    else {
      res[i].p = PolyNegMonos(PolyGetSize(&monos[i].p), PolyGetArr(&monos[i].p));
    }
    res[i].exp = MonoGetExp(&monos[i]);
  }
  return PolyFromMonos(res, count);
}

Poly PolyNeg(const Poly *p) {
  assert(p);
  Poly res;
  if (PolyIsCoeff(p))
    res = PolyFromCoeff(NegCoeff(PolyGetCoeff(p)));
  else
    res = PolyNegMonos(PolyGetSize(p), PolyGetArr(p));
  return PolySimplify(&res);
}

Poly PolySub(const Poly *p, const Poly *q) {
  assert(p && q);
  Poly q_neg = PolyNeg(q);
  Poly res = PolyAdd(p, &q_neg);
  PolyDestroy(&q_neg);
  return PolySimplify(&res);
}

/**
 * Mnoży dwie tablice jednomianów @p p i @p q.
 * @param p_monos : tablica jednomianów
 * @param p_size : liczba jednomianów
 * @param q_monos : tablica jednomianów
 * @param q_size : liczba jednomianów
 * @return wielomian będący wynikiem mnożenia   
 */
static Poly PolyMulMonos(const Mono p_monos[], size_t p_size, const Mono q_monos[], size_t q_size) {
  assert(p_monos && q_monos);
  Mono *res;
  size_t res_size = p_size * q_size;
  res = AllocMemForMonos(res_size);
  size_t res_i = 0;
  for (size_t p_i = 0; p_i < p_size; p_i++) {
    for (size_t q_i = 0; q_i < q_size; q_i++) {
      res[res_i].p = PolyMul(&p_monos[p_i].p, &q_monos[q_i].p);
      res[res_i].exp = MonoGetExp(&p_monos[p_i]) + MonoGetExp(&q_monos[q_i]);
      res_i++;
    }
  }
  return PolyFromMonos(res, res_size);
}

/**
 * Zwraca wielomian, który jest wynikiem
 * mnożenia tablicy jednomianów przez
 * wielomian, który jest współczynnikiem.
 * @param monos : tablica jednomianów
 * @param count : liczba jednomianów
 * @param p : wielomian
 * @return wielomian będący wynikiem mnożenia
 */
static Poly PolyMulMonosByPoly(const Mono monos[], size_t count, const Poly *p) {
  assert(monos && PolyIsCoeff(p));
  Mono *res = AllocMemForMonos(count);
  for (size_t i = 0; i < count; i++) {
    res[i].p = PolyMul(&monos[i].p, p);
    res[i].exp = MonoGetExp(&monos[i]);
  }
  return PolyFromMonos(res, count);
}

Poly PolyMul(const Poly *p, const Poly *q) {
  assert(p && q);
  Poly res;
  if (PolyIsZero(p) || PolyIsZero(q)) {
    res = PolyZero();
  } else if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
    res = PolyFromCoeff(PolyGetCoeff(p) * PolyGetCoeff(q));
  } else if (PolyIsCoeff(p) && !PolyIsCoeff(q)) {
    res = PolyMulMonosByPoly(PolyGetArr(q), PolyGetSize(q), p);
  } else if (!PolyIsCoeff(p) && PolyIsCoeff(q)) {
    res = PolyMulMonosByPoly(PolyGetArr(p), PolyGetSize(p), q);
  } else {
    res = PolyMulMonos(PolyGetArr(p), PolyGetSize(p), PolyGetArr(q), PolyGetSize(q));
  } if (!PolyIsCoeff(&res)) {
    Mono *temp = PolyGetArr(&res);
    res = PolyAddMonos(PolyGetSize(&res), PolyGetArr(&res));
    free(temp);
  }
    return PolySimplify(&res);
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
  assert(p);
  poly_exp_t res = -1;
  if (PolyIsZero(p)) {
    res = -1;
  } else if (PolyIsCoeff(p)) {
    res = 0;
  } else if (var_idx == 0) {
    for (size_t i = 0; i < PolyGetSize(p); i++)
      if (!PolyIsZero(&p->arr[i].p))
        res = ExpMax(res, MonoGetExp(&p->arr[i]));
  } else {
    for (size_t i = 0; i < PolyGetSize(p); i++)
      res = ExpMax(res, PolyDegBy(&p->arr[i].p, var_idx - 1));
  }
  return res;
}

poly_exp_t PolyDeg(const Poly *p) {
  assert(p);
  poly_exp_t res = 0;
  if (PolyIsZero(p)) {
    res = -1;
  } else if (!PolyIsCoeff(p)) {
    for (size_t i = 0; i < PolyGetSize(p); i++)
      if (!PolyIsZero(&p->arr[i].p))
        res = ExpMax(MonoGetExp(&p->arr[i]) + PolyDeg(&p->arr[i].p), res);
  }
  return res;
}

/**
 * Zwraca wartość stałej @p x
 * podniesionej do potęgi @p n.
 * @param x : współczynnik wielomianu
 * @param n : wykładnik
 * @return @f$x^n@f$
 */
static poly_coeff_t ExpBySquaring(poly_coeff_t x, poly_exp_t n) {
  poly_coeff_t res = 1;
  while (n > 0) {
    if (n & 1)
      res *= x;
    x *= x;
    n /= 2;
  }
  return res;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
  assert(p);
  Poly res;
  if (PolyIsCoeff(p)) {
    res = PolyFromCoeff(PolyGetCoeff(p));
  } else {
    poly_coeff_t coeff = ExpBySquaring(x, MonoGetExp(&p->arr[0]));
    Poly temp1 = PolyFromCoeff(coeff), temp2;
    res = PolyMul(&p->arr[0].p, &temp1);
    for (size_t i = 1; i < PolyGetSize(p); i++) {
      coeff = ExpBySquaring(x, MonoGetExp(&p->arr[i]));
      temp1 = PolyFromCoeff(coeff);
      temp1 = PolyMul(&p->arr[i].p, &temp1);
      temp2 = res;
      res = PolyAdd(&temp2, &temp1);
      PolyDestroy(&temp2);
      PolyDestroy(&temp1);
    }
  }
  return PolySimplify(&res);
}
// zamienia wiel z samymi exp = 0 na coeff
//przejmuje na własność @p p
// moze zmienic na void
Poly PolySimplify(Poly *p) {
  /*
  printf("before simplifying:\t");
  PolyPrint(p);
  printf("\n");*/
  Poly res, *temp = p;
  while (!PolyIsCoeff(temp) && PolyGetSize(temp) == 1 && MonoGetExp(&temp->arr[0]) == 0)
    temp = &temp->arr[0].p;
  if (PolyIsCoeff(temp)) {
    res = PolyFromCoeff(PolyGetCoeff(temp));
    PolyDestroy(p);
  }
  else {
    res.arr = PolyGetArr(p);
    if (PolyIsCoeff(p))
      res.coeff = PolyGetCoeff(p);
    else
      res.size = PolyGetSize(p);
  } /*
  printf("after simplifying:\t");
  PolyPrint(&res);
  printf("\n"); */
  return res;
}

void PolyPrint(const Poly *p) {
  assert(p);
  if (PolyIsCoeff(p))
    printf("%ld", PolyGetCoeff(p));
  else {
    size_t p_size = PolyGetSize(p);
    for (size_t i = 0; i < p_size - 1; i++) {
      printf("(");
      PolyPrint(&p->arr[i].p);
      printf(",%d)", MonoGetExp(&p->arr[i]));
      printf("+");
    }
    printf("(");
    PolyPrint(&p->arr[p_size - 1].p);
    printf(",%d)", MonoGetExp(&p->arr[p_size - 1]));
  }
}