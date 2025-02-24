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
#ifndef soft_H
#define soft_H

/** \file
 *  Routines to calculate the soft-sphere potential between particle pairs.
 *
 *  Implementation in \ref soft_sphere.cpp
 */

#include "config.hpp"

#ifdef SOFT_SPHERE

#include "nonbonded_interaction_data.hpp"

#include <utils/Vector.hpp>

#include <cmath>

int soft_sphere_set_params(int part_type_a, int part_type_b, double a, double n,
                           double cut, double offset);

/** Calculate soft-sphere force factor */
inline double soft_pair_force_factor(IA_parameters const &ia_params,
                                     double dist) {
  if (dist < (ia_params.soft_sphere.cut + ia_params.soft_sphere.offset)) {
    /* normal case: resulting force/energy smaller than zero. */
    auto const r_off = dist - ia_params.soft_sphere.offset;
    if (r_off > 0.0) {
      auto const n = ia_params.soft_sphere.n;
      return ia_params.soft_sphere.a * n / std::pow(r_off, n + 1) / dist;
    }
  }
  return 0.0;
}

/** Calculate soft-sphere energy */
inline double soft_pair_energy(IA_parameters const &ia_params, double dist) {
  if (dist < (ia_params.soft_sphere.cut + ia_params.soft_sphere.offset)) {
    auto const r_off = dist - ia_params.soft_sphere.offset;
    /* normal case: resulting force/energy smaller than zero. */
    return ia_params.soft_sphere.a / std::pow(r_off, ia_params.soft_sphere.n);
  }
  return 0.0;
}

#endif /* ifdef SOFT_SPHERE */
#endif
