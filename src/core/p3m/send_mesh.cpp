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

#if defined(P3M) || defined(DP3M)

#include "p3m/common.hpp"
#include "p3m/fft.hpp"
#include "p3m/send_mesh.hpp"

#include <utils/Span.hpp>
#include <utils/Vector.hpp>
#include <utils/mpi/cart_comm.hpp>

#include <boost/mpi/communicator.hpp>

#include <mpi.h>

#include <cstddef>
#include <utility>

/** Add values of a 3d-grid input block (size[3]) to values of 3d-grid
 *  output array with dimension dim[3] at start position start[3].
 *
 *  \param in          Pointer to first element of input block data.
 *  \param out         Pointer to first element of output grid.
 *  \param start       Start position of block in output grid.
 *  \param size        Dimensions of the block
 *  \param dim         Dimensions of the output grid.
 */
static void p3m_add_block(double const *in, double *out, int const start[3],
                          int const size[3], int const dim[3]) {
  /* fast, mid and slow changing indices */
  int f, m, s;
  /* linear index of in grid, linear index of out grid */
  int li_in = 0, li_out = 0;
  /* offsets for indices in output grid */
  int m_out_offset, s_out_offset;

  li_out = start[2] + (dim[2] * (start[1] + (dim[1] * start[0])));
  m_out_offset = dim[2] - size[2];
  s_out_offset = (dim[2] * (dim[1] - size[1]));

  for (s = 0; s < size[0]; s++) {
    for (m = 0; m < size[1]; m++) {
      for (f = 0; f < size[2]; f++) {
        out[li_out++] += in[li_in++];
      }
      li_out += m_out_offset;
    }
    li_out += s_out_offset;
  }
}

void p3m_send_mesh::resize(const boost::mpi::communicator &comm,
                           const P3MLocalMesh &local_mesh) {
  int done[3] = {0, 0, 0};
  /* send grids */
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      /* left */
      s_ld[i * 2][j] = 0 + done[j] * local_mesh.margin[j * 2];
      if (j == i)
        s_ur[i * 2][j] = local_mesh.margin[j * 2];
      else
        s_ur[i * 2][j] =
            local_mesh.dim[j] - done[j] * local_mesh.margin[(j * 2) + 1];
      /* right */
      if (j == i)
        s_ld[(i * 2) + 1][j] = local_mesh.in_ur[j];
      else
        s_ld[(i * 2) + 1][j] = 0 + done[j] * local_mesh.margin[j * 2];
      s_ur[(i * 2) + 1][j] =
          local_mesh.dim[j] - done[j] * local_mesh.margin[(j * 2) + 1];
    }
    done[i] = 1;
  }
  max = 0;
  for (int i = 0; i < 6; i++) {
    s_size[i] = 1;
    for (int j = 0; j < 3; j++) {
      s_dim[i][j] = s_ur[i][j] - s_ld[i][j];
      s_size[i] *= s_dim[i][j];
    }
    if (s_size[i] > max)
      max = s_size[i];
  }
  /* communication */
  auto const node_neighbors = Utils::Mpi::cart_neighbors<3>(comm);

  int r_margin[6];
  for (int i = 0; i < 6; i++) {
    auto const j = (i % 2 == 0) ? i + 1 : i - 1;

    if (node_neighbors[i] != comm.rank()) {
      MPI_Sendrecv(&(local_mesh.margin[i]), 1, MPI_INT, node_neighbors[i],
                   REQ_P3M_INIT, &(r_margin[j]), 1, MPI_INT, node_neighbors[j],
                   REQ_P3M_INIT, comm, MPI_STATUS_IGNORE);
    } else {
      r_margin[j] = local_mesh.margin[i];
    }
  }
  /* recv grids */
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) {
      if (j == i) {
        r_ld[i * 2][j] = s_ld[i * 2][j] + local_mesh.margin[2 * j];
        r_ur[i * 2][j] = s_ur[i * 2][j] + r_margin[2 * j];
        r_ld[(i * 2) + 1][j] = s_ld[(i * 2) + 1][j] - r_margin[(2 * j) + 1];
        r_ur[(i * 2) + 1][j] =
            s_ur[(i * 2) + 1][j] - local_mesh.margin[(2 * j) + 1];
      } else {
        r_ld[i * 2][j] = s_ld[i * 2][j];
        r_ur[i * 2][j] = s_ur[i * 2][j];
        r_ld[(i * 2) + 1][j] = s_ld[(i * 2) + 1][j];
        r_ur[(i * 2) + 1][j] = s_ur[(i * 2) + 1][j];
      }
    }
  for (int i = 0; i < 6; i++) {
    r_size[i] = 1;
    for (int j = 0; j < 3; j++) {
      r_dim[i][j] = r_ur[i][j] - r_ld[i][j];
      r_size[i] *= r_dim[i][j];
    }
    if (r_size[i] > max)
      max = r_size[i];
  }
}

void p3m_send_mesh::gather_grid(Utils::Span<double *> meshes,
                                const boost::mpi::communicator &comm,
                                const Utils::Vector3i &dim) {
  auto const node_neighbors = Utils::Mpi::cart_neighbors<3>(comm);
  send_grid.resize(max * meshes.size());
  recv_grid.resize(max * meshes.size());

  /* direction loop */
  for (int s_dir = 0; s_dir < 6; s_dir++) {
    auto const r_dir = (s_dir % 2 == 0) ? s_dir + 1 : s_dir - 1;

    /* pack send block */
    if (s_size[s_dir] > 0)
      for (std::size_t i = 0; i < meshes.size(); i++) {
        fft_pack_block(meshes[i], send_grid.data() + i * s_size[s_dir],
                       s_ld[s_dir], s_dim[s_dir], dim.data(), 1);
      }

    /* communication */
    if (node_neighbors[s_dir] != comm.rank()) {
      MPI_Sendrecv(
          send_grid.data(), static_cast<int>(meshes.size()) * s_size[s_dir],
          MPI_DOUBLE, node_neighbors[s_dir], REQ_P3M_GATHER, recv_grid.data(),
          static_cast<int>(meshes.size()) * r_size[r_dir], MPI_DOUBLE,
          node_neighbors[r_dir], REQ_P3M_GATHER, comm, MPI_STATUS_IGNORE);
    } else {
      std::swap(send_grid, recv_grid);
    }
    /* add recv block */
    if (r_size[r_dir] > 0) {
      for (std::size_t i = 0; i < meshes.size(); i++) {
        p3m_add_block(recv_grid.data() + i * r_size[r_dir], meshes[i],
                      r_ld[r_dir], r_dim[r_dir], dim.data());
      }
    }
  }
}

void p3m_send_mesh::spread_grid(Utils::Span<double *> meshes,
                                const boost::mpi::communicator &comm,
                                const Utils::Vector3i &dim) {
  auto const node_neighbors = Utils::Mpi::cart_neighbors<3>(comm);
  send_grid.resize(max * meshes.size());
  recv_grid.resize(max * meshes.size());

  /* direction loop */
  for (int s_dir = 5; s_dir >= 0; s_dir--) {
    auto const r_dir = (s_dir % 2 == 0) ? s_dir + 1 : s_dir - 1;

    /* pack send block */
    if (r_size[r_dir] > 0)
      for (std::size_t i = 0; i < meshes.size(); i++) {
        fft_pack_block(meshes[i], send_grid.data() + i * r_size[r_dir],
                       r_ld[r_dir], r_dim[r_dir], dim.data(), 1);
      }
    /* communication */
    if (node_neighbors[r_dir] != comm.rank()) {
      MPI_Sendrecv(
          send_grid.data(), r_size[r_dir] * static_cast<int>(meshes.size()),
          MPI_DOUBLE, node_neighbors[r_dir], REQ_P3M_SPREAD, recv_grid.data(),
          s_size[s_dir] * static_cast<int>(meshes.size()), MPI_DOUBLE,
          node_neighbors[s_dir], REQ_P3M_SPREAD, comm, MPI_STATUS_IGNORE);
    } else {
      std::swap(send_grid, recv_grid);
    }
    /* un pack recv block */
    if (s_size[s_dir] > 0) {
      for (std::size_t i = 0; i < meshes.size(); i++) {
        fft_unpack_block(recv_grid.data() + i * s_size[s_dir], meshes[i],
                         s_ld[s_dir], s_dim[s_dir], dim.data(), 1);
      }
    }
  }
}

#endif
