# Copyright (C) 2019-2022 The ESPResSo project
#
# This file is part of ESPResSo.
#
# ESPResSo is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ESPResSo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import unittest as ut
import importlib_wrapper
import pathlib

sample, skipIfMissingFeatures = importlib_wrapper.configure_and_import(
    "@SAMPLES_DIR@/ekboundaries.py", gpu=True, n_int_cycles=50)


@skipIfMissingFeatures
class Sample(ut.TestCase):
    system = sample.system

    def test_file_generation(self):
        # test .vtk files exist
        path_vtk_root = pathlib.Path("ek")
        for basename in ["pos_dens_0.vtk", "pos_flux_0.vtk", "ekv_0.vtk",
                         "neg_dens_0.vtk", "neg_flux_0.vtk", "ekb_0.vtk"]:
            filepath = path_vtk_root / basename
            self.assertTrue(filepath.is_file(), f"File {filepath} not created")


if __name__ == "__main__":
    ut.main()
