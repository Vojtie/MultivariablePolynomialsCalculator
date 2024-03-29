/** @file
 * Implementacja interfejsu klasy wielomianów rzadkich wielu zmiennych.
 *
 * @authors Wojciech Kuzebski <wk429552@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2021
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "poly.h"

/**
 * Sprawdza czy alokacja pamięci się powiodła.
 * Jeśli nie to kończy program awaryjnie kodem 1.
 * @param[in] pointer : wskaźnik
 */
static void CheckAlloc(const void *pointer) {
  if (!pointer)
    exit(7);
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

Mono *AllocMemForMonos(size_t count) {
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
/**** UPDATE ****/
/**
 * Zwraca wielomian będący sumą wielomianu i stałej.
 * @param[in] p : wielomian
 * @param[in] coeff : stała
 * @return @f$p + coeff@f$
 */
static Poly PolyAddCoeffToMonos(const Poly *p, poly_coeff_t coeff) {
  assert(p && !PolyIsCoeff(p));
  Poly res;
  if (coeff == 0) {
    res = PolyClone(p);
  } else if (MonoGetExp(&p->arr[0]) == 0) {
    if (PolyIsCoeff(MonoGetPtrToPoly(&p->arr[0])) && coeff + PolyGetCoeff(MonoGetPtrToPoly(&p->arr[0])) == 0) {
      assert(PolyGetSize(p) > 1);
      res.size = PolyGetSize(p) - 1;
      res.arr = AllocMemForMonos(res.size);
      for (size_t i = 0; i < res.size; i++)
        res.arr[i] = MonoClone(&p->arr[i + 1]);
    } else {
      res.arr = AllocMemForMonos(PolyGetSize(p));
      res.size = PolyGetSize(p);
      for (size_t i = 1; i < res.size; i++)
        res.arr[i] = MonoClone(&p->arr[i]);
      if (PolyIsCoeff(MonoGetPtrToPoly(&p->arr[0])))
        res.arr[0].p = PolyFromCoeff(coeff + PolyGetCoeff(MonoGetPtrToPoly(&p->arr[0])));
      else
        res.arr[0].p = PolyAddCoeffToMonos(MonoGetPtrToPoly(&p->arr[0]), coeff);
      res.arr[0].exp = 0;
    }
  } else {
    res.size = PolyGetSize(p) + 1;
    res.arr = AllocMemForMonos(res.size);
    res.arr[0].exp = 0;
    res.arr[0].p = PolyFromCoeff(coeff);
    for (size_t i = 1; i < res.size; i++)
      res.arr[i] = MonoClone(&p->arr[i - 1]);
  }
  return res;
}

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
    res.size = 0;
    size_t p_size = PolyGetSize(p), q_size = PolyGetSize(q);
    size_t p_i = 0, q_i = 0;
    while (p_i < p_size && q_size > q_i) {
      assert(p_i == 0 || MonoGetExp(&p->arr[p_i]) > MonoGetExp(&p->arr[p_i - 1]));
      assert(q_i == 0 || MonoGetExp(&q->arr[q_i]) > MonoGetExp(&q->arr[q_i - 1]));
      if (MonoGetExp(&p->arr[p_i]) == MonoGetExp(&q->arr[q_i])) {
        p_i++;
        q_i++;
      } else if (MonoGetExp(&p->arr[p_i]) > MonoGetExp(&q->arr[q_i]))
        q_i++;
      else p_i++;
      res.size++;
    }
    while (p_i < p_size) {
      res.size++;
      p_i++;
    }
    while (q_i < q_size) {
      res.size++;
      q_i++;
    }
    res.arr = AllocMemForMonos(res.size);
    size_t i = 0;
    for (p_i = 0, q_i = 0; p_i < p_size && q_size > q_i; i++) {
      if (MonoGetExp(&p->arr[p_i]) == MonoGetExp(&q->arr[q_i])) {
        res.arr[i].exp = MonoGetExp(&p->arr[p_i]);
        res.arr[i].p = PolyAdd(MonoGetPtrToPoly(&p->arr[p_i]), MonoGetPtrToPoly(&q->arr[q_i]));
        p_i++;
        q_i++;
      } else if (MonoGetExp(&p->arr[p_i]) > MonoGetExp(&q->arr[q_i])) {
        res.arr[i] = MonoClone(&q->arr[q_i]);
        q_i++;
      } else {
        res.arr[i] = MonoClone(&p->arr[p_i]);
        p_i++;
      }
    }
    while (p_i < p_size)
      res.arr[i++] = MonoClone(&p->arr[p_i++]);
    while (q_i < q_size)
      res.arr[i++] = MonoClone(&q->arr[q_i++]);
  }
  if (!PolyIsCoeff(&res))
    res = PolyDelZeros(&res);
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

Poly PolyAddMonos(size_t count, const Mono monos[]) {
  Poly res;
  if (count == 0 || monos == NULL)
    res = PolyZero();
  else {
    Mono *copys = AllocMemForMonos(count);
    memcpy(copys, monos, sizeof(Mono) * count);
    res = PolyOwnMonos(count, copys);
  }
  return res;
}

Poly PolyOwnMonos(size_t count, Mono *monos) {
  Poly res;
  if (count == 0 || monos == NULL)
    res = PolyZero();
  else {
    SortMonos(monos, count);
    for (size_t i = 0; i < count; i++) {
      if (!PolyIsZero(&monos[i].p)) {
        for (size_t j = i + 1; j < count
          && MonoGetExp(&monos[i]) == MonoGetExp(&monos[j]); j++) {
          Poly temp = monos[i].p;
          monos[i].p = PolyAdd(&monos[i].p, &monos[j].p);
          PolyDestroy(&temp);
          PolyDestroy(&monos[j].p);
          monos[j].p = PolyZero();
        }
      }
    }
    res = PolyFromMonos(monos, count);
    res = PolyDelZeros(&res);
  }
  return res;
}

Poly PolyCloneMonos(size_t count, const Mono monos[]) {
  Poly res;
  if (count == 0 || monos == NULL)
    res = PolyZero();
  else {
    Mono *copys = AllocMemForMonos(count);
    for (size_t i = 0; i < count; i++)
      copys[i] = MonoClone(monos + i);
    res = PolyOwnMonos(count, copys);
  }
  return res;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
  assert(p && q);
  bool res = true;
  if (PolyIsCoeff(p) && PolyIsCoeff(q))
    res = PolyGetCoeff(p) == PolyGetCoeff(q);
  else if (!PolyIsCoeff(p) && PolyIsCoeff(q)) {
    if (PolyGetSize(p) == 1 && MonoGetExp(&p->arr[0]) == 0)
      res = PolyGetCoeff(MonoGetPtrToPoly(&p->arr[0])) == PolyGetCoeff(q);
    else res = false;
  }
  else if (PolyIsCoeff(p) && !PolyIsCoeff(q)) {
    if (PolyGetSize(q) == 1 && MonoGetExp(&q->arr[0]) == 0)
      res = PolyGetCoeff(MonoGetPtrToPoly(&q->arr[0])) == PolyGetCoeff(p);
    else res = false;
  }
  else if (PolyGetSize(p) == PolyGetSize(q)) {
    size_t size = PolyGetSize(p);
    for (size_t i = 0; res && i < size; i++)
      if (!PolyIsEq(MonoGetPtrToPoly(&p->arr[0]), MonoGetPtrToPoly(&q->arr[0]))
          || MonoGetExp(&p->arr[0]) != MonoGetExp(&q->arr[0]))
        res = false;
  }
  else res = false;
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
  return res;
}

Poly PolySub(const Poly *p, const Poly *q) {
  assert(p && q);
  Poly q_neg = PolyNeg(q);
  Poly res = PolyAdd(p, &q_neg);
  PolyDestroy(&q_neg);
  return res;
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
    return res;
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

/**
 * Zwraca wielomian @p p
 * podniesiony do potęgi @p exp.
 * @param p : wielomian
 * @param exp : wykładnik potęgi
 * @return @f$p^{exp}@f$
 */
Poly PolyRaiseToPower(const Poly *p, poly_exp_t exp) {
  assert(exp >= 0);
  Poly res;
  if (exp == 0)
    res = PolyFromCoeff(1);
  else if (p == NULL)
    res = PolyZero();
  else if (exp == 1)
    res = PolyClone(p);
  else {
    res = PolyFromCoeff(1);
    Poly p_clone = PolyClone(p);
    while (exp > 0) {
      if (exp & 1) {
        Poly temp = res;
        res = PolyMul(&res, &p_clone);
        PolyDestroy(&temp);
      }
      Poly temp = p_clone;
      p_clone = PolyMul(&p_clone, &p_clone);
      PolyDestroy(&temp);
      exp /= 2;
    }
    PolyDestroy(&p_clone);
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
  return res;
}

Poly PolySimplify(Poly *p) {
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
  }
  return res;
}

Poly PolySimplifyRec(Poly *p) {
  Poly res = PolySimplify(p);
  if (!PolyIsCoeff(&res)) {
    size_t size = PolyGetSize(p);
    for (size_t i = 0; i < size; i++)
      res.arr[i].p = PolySimplifyRec(MonoGetPtrToPoly(&res.arr[i]));
    res.size = size;
  }
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

Poly PolyAddPolys(size_t count, const Poly polys[]) {
  size_t size = 0;
  for (size_t i = 0; i < count; i++)
    if (!PolyIsCoeff(polys + i))
      size += PolyGetSize(polys + i);
  Mono monos[size];
  poly_coeff_t coeff = 0;
  for (size_t polys_i = 0, m_i = 0; polys_i < count; polys_i++) {
    if (!PolyIsCoeff(polys + polys_i)) {
      size_t p_i_size = PolyGetSize(polys + polys_i);
      for (size_t j = 0; j < p_i_size; j++)
        monos[m_i++] = polys[polys_i].arr[j];
      free(polys[polys_i].arr);
    } else {
      coeff += PolyGetCoeff(polys + polys_i);
    }
  }
  Poly res = PolyAddMonos(size, monos), temp = res;
  Poly poly_coeff = PolyFromCoeff(coeff);
  res = PolyAdd(&res, &poly_coeff);
  PolyDestroy(&temp);
  return res;
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[]) {
  assert(p);
  Poly composed;
  size_t p_size = PolyGetSize(p);
  if (PolyIsCoeff(p))
    composed = PolyFromCoeff(PolyGetCoeff(p));
  else {
    Poly polys[p_size];
    for (size_t i = 0; i < p_size; i++) {
      Poly temp1;
      if (k == 0)
        temp1 = PolyCompose(&p->arr[i].p, k, NULL);
      else
        temp1 = PolyCompose(&p->arr[i].p, k - 1, q + 1);
      if (PolyIsZero(&temp1))
        polys[i] = temp1;
      else {
        Poly temp2 = PolyRaiseToPower(k == 0 ? NULL : q, MonoGetExp(p->arr + i));
        polys[i] = PolyMul(&temp1, &temp2);
        PolyDestroy(&temp1);
        PolyDestroy(&temp2);
      }
    }
    composed = PolyAddPolys(p_size, polys);
  }
  return composed;
}
