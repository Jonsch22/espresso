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
 *  Implementation of \ref lj.hpp
 */
#include "lj.hpp"

#ifdef LENNARD_JONES
#include "interactions.hpp"
#include "nonbonded_interaction_data.hpp"

#include <utils/constants.hpp>

int lennard_jones_set_params(int part_type_a, int part_type_b, double eps,
                             double sig, double cut, double shift,
                             double offset, double min) {
  IA_parameters *data = get_ia_param_safe(part_type_a, part_type_b);

  if (!data)
    return ES_ERROR;

  data->lj.eps = eps;
  data->lj.sig = sig;
  data->lj.cut = cut;
  data->lj.shift = shift;
  data->lj.offset = offset;
  if (min > 0) {
    data->lj.min = min;
  }
  /* broadcast interaction parameters */
  mpi_bcast_ia_params(part_type_a, part_type_b);

  return ES_OK;
}

#endif /* ifdef LENNARD_JONES */
