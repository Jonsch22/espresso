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

#ifndef ESPRESSO_CELLSTRUCTURETYPE_HPP
#define ESPRESSO_CELLSTRUCTURETYPE_HPP

/** Cell structure */
enum class CellStructureType : int {
  /** cell structure regular decomposition */
  CELL_STRUCTURE_REGULAR = 1,
  /** cell structure n square */
  CELL_STRUCTURE_NSQUARE = 2,
  /** cell structure hybrid */
  CELL_STRUCTURE_HYBRID = 3
};

#endif // ESPRESSO_CELLSTRUCTURETYPE_HPP
