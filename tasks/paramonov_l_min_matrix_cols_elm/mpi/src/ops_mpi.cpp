#include "paramonov_l_min_matrix_cols_elm/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "paramonov_l_min_matrix_cols_elm/common/include/common.hpp"
#include "util/include/util.hpp"

namespace paramonov_l_min_matrix_cols_elm {

ParamonovLMinMatrixColsElmMPI::ParamonovLMinMatrixColsElmMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool ParamonovLMinMatrixColsElmMPI::ValidationImpl() {
  std::size_t m = std::get<0>(GetInput());
  std::size_t n = std::get<1>(GetInput());
  std::vector<int> &val = std::get<2>(GetInput());
  valid_ = (n > 0) && (m > 0) && (val.size() == (n * m));
  return valid_;
}

bool ParamonovLMinMatrixColsElmMPI::PreProcessingImpl() {
  if (valid_) {
    t_matrix_ = std::get<2>(GetInput());
    return true;
  }
  return false;
}

bool ParamonovLMinMatrixColsElmMPI::RunImpl() {
  if (!valid_) {
    return false;
  }

  std::size_t m = std::get<0>(GetInput());
  std::size_t n = std::get<1>(GetInput());

  std::string debug = "\n\n-----------\n";
  for (std::size_t i = 0; i < n; i++) {
    for (std::size_t j = 0; j < m; j++) {
      debug += std::to_string(t_matrix_[(i * m) + j]) + " ";
    }
    debug += "\n";
  }
  std::cout << debug;

  int rank, mpi_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  int cols_per_process = static_cast<int>(m) / mpi_size;
  int start_col = rank * cols_per_process;
  int end_col = (rank == mpi_size - 1) ? static_cast<int>(m) : start_col + cols_per_process;

  std::vector<int> local_mins((end_col - start_col) * n);

  for (int col = start_col; col < end_col; col++) {
    for (std::size_t row = 0; row < n; row++) {
      int local_idx = (col - start_col) * static_cast<int>(n) + static_cast<int>(row);
      if (row == 0) {
        local_mins[local_idx] = t_matrix_[row * m + col];
      } else {
        local_mins[local_idx] = std::min(local_mins[local_idx], t_matrix_[row * m + col]);
      }
    }
  }

  std::vector<int> all_mins;
  if (rank == 0) {
    all_mins.resize(m * n);
  }

  std::vector<int> recv_counts(mpi_size);
  std::vector<int> displs(mpi_size);
  int local_size = static_cast<int>(local_mins.size());
  
  MPI_Gather(&local_size, 1, MPI_INT, recv_counts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  if (rank == 0) {
    displs[0] = 0;
    for (int i = 1; i < mpi_size; i++) {
      displs[i] = displs[i-1] + recv_counts[i-1];
    }
  }

  MPI_Gatherv(local_mins.data(), local_size, MPI_INT,
              all_mins.data(), recv_counts.data(), displs.data(), MPI_INT,
              0, MPI_COMM_WORLD);

  std::vector<int> result;
  if (rank == 0) {
    result.resize(m);
    for (int col = 0; col < static_cast<int>(m); col++) {
      int process_id = col / cols_per_process;
      if (process_id >= mpi_size) process_id = mpi_size - 1;
      
      int local_col = col - (process_id * cols_per_process);
      int start_idx = displs[process_id] + local_col * static_cast<int>(n);
      
      result[col] = all_mins[start_idx];
      for (std::size_t row = 1; row < n; row++) {
        result[col] = std::min(result[col], all_mins[start_idx + static_cast<int>(row)]);
      }
    }
  }

  if (rank == 0) {
    GetOutput() = result;
  } else {
    GetOutput().resize(m);
  }

  MPI_Bcast(GetOutput().data(), static_cast<int>(m), MPI_INT, 0, MPI_COMM_WORLD);

  return true;
}

bool ParamonovLMinMatrixColsElmMPI::PostProcessingImpl() {
  return true;
}

}  // namespace paramonov_l_min_matrix_cols_elm