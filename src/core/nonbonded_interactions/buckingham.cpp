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
/** \file
 *
 *  Implementation of \ref buckingham.hpp
 */
#include "buckingham.hpp"

#ifdef BUCKINGHAM
#include "interactions.hpp"
#include "nonbonded_interaction_data.hpp"

#include <utils/constants.hpp>

int buckingham_set_params(int part_type_a, int part_type_b, double A, double B,
                          double C, double D, double cut, double discont,
                          double shift) {
  IA_parameters *data = get_ia_param_safe(part_type_a, part_type_b);

  if (!data)
    return ES_ERROR;

  data->buckingham.A = A;
  data->buckingham.B = B;
  data->buckingham.C = C;
  data->buckingham.D = D;
  data->buckingham.cut = cut;
  data->buckingham.discont = discont;
  data->buckingham.shift = shift;

  /* Replace the Buckingham potential for interatomic distance less
     than or equal to discontinuity by a straight line (F1+F2*r) */

  auto const F = buck_force_r(A, B, C, D, discont);
  data->buckingham.F1 = buck_energy_r(A, B, C, D, shift, discont) + discont * F;
  data->buckingham.F2 = -F;

  /* broadcast interaction parameters */
  mpi_bcast_ia_params(part_type_a, part_type_b);

  return ES_OK;
}

#endif
