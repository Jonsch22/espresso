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

#include "config.hpp"

#ifdef EXCLUSIONS

#include "exclusions.hpp"

#include "Particle.hpp"

#include <utils/contains.hpp>

#include <algorithm>

void add_exclusion(Particle &p, int p_id) {
  if (Utils::contains(p.exclusions(), p_id))
    return;

  p.exclusions().push_back(p_id);
}

void delete_exclusion(Particle &p, int p_id) {
  auto &el = p.exclusions();

  el.erase(std::remove(el.begin(), el.end(), p_id), el.end());
}

#endif // EXCLUSIONS
