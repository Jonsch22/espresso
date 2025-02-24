/*
 * Copyright (C) 2010-2022 The ESPResSo project
 * Copyright (C) 2002,2003,2004,2005,2006,2007,2008,2009,2010
 *   Max-Planck-Institute for Polymer Research, Theory Group
 *
 * This file is part of ESPResSo.
 *
 * ESPResSo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ESPResSo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  This file contains implementations for some special functions which are
 *  needed by the MMM family of algorithms. This are the modified Hurwitz zeta
 *  function and the modified Bessel functions of first and second kind. The
 *  implementations are based on the GSL code (see @ref specfunc.cpp for the
 *  original GSL header).
 *
 *  The Hurwitz zeta function is evaluated using the Euler-Maclaurin summation
 *  formula, the Bessel functions are evaluated using several different
 *  Chebychev expansions. Both achieve a precision of nearly machine precision,
 *  which is no problem for the Hurwitz zeta function, which is only used when
 *  determining the coefficients for the modified polygamma functions (see @ref
 *  mmm-common.hpp).
 */

#ifndef ESPRESSO_SRC_CORE_ELECTROSTATICS_SPECFUNC_HPP
#define ESPRESSO_SRC_CORE_ELECTROSTATICS_SPECFUNC_HPP

#include <utils/Span.hpp>

#include <cassert>
#include <utility>

/** Hurwitz zeta function. This function was taken from the GSL code. */
double hzeta(double order, double x);

/** Modified Bessel function of second kind, order 0. This function was taken
 *  from the GSL code. Precise roughly up to machine precision.
 *  If @c MMM1D_MACHINE_PREC is not defined, @ref LPK0 is used instead.
 */
double K0(double x);

/** Modified Bessel function of second kind, order 1. This function was taken
 *  from the GSL code. Precise roughly up to machine precision.
 *  If @c MMM1D_MACHINE_PREC is not defined, @ref LPK1 is used instead.
 */
double K1(double x);

/** Bessel function of second kind, order 0, low precision.
 *  The implementation has an absolute precision of around 10^(-14), which is
 *  comparable to the relative precision sqrt implementation of current
 *  hardware.
 */
double LPK0(double x);

/** Bessel function of second kind, order 1, low precision.
 *  The implementation has an absolute precision of around 10^(-14), which is
 *  comparable to the relative precision sqrt implementation of current
 *  hardware.
 */
double LPK1(double x);

/** Bessel functions of second kind, order 0 and order 1, low precision.
 *  The implementation has an absolute precision of around 10^(-14), which is
 *  comparable to the relative precision sqrt implementation of current
 *  hardware.
 */
std::pair<double, double> LPK01(double x);

/** Evaluate the polynomial interpreted as a Taylor series via the
 *  Horner scheme.
 */
inline double evaluateAsTaylorSeriesAt(Utils::Span<const double> series,
                                       double x) {
  assert(not series.empty());
  auto cnt = static_cast<int>(series.size()) - 1;
  const double *c = series.data();
  double r = c[cnt];
  while (--cnt >= 0)
    r = r * x + c[cnt];
  return r;
}

/** Evaluate the polynomial interpreted as a Chebychev series. Requires a
 *  series with at least three coefficients, i.e. no linear approximations!
 */
inline double evaluateAsChebychevSeriesAt(Utils::Span<const double> series,
                                          double x) {
  assert(series.size() >= 3);

  const double *c = series.data();
  double const x2 = 2.0 * x;
  double dd = c[series.size() - 1];
  double d = x2 * dd + c[series.size() - 2];
  for (auto j = static_cast<int>(series.size()) - 3; j >= 1; j--) {
    auto const tmp = d;
    d = x2 * d - dd + c[j];
    dd = tmp;
  }
  return x * d - dd + 0.5 * c[0];
}

#endif
