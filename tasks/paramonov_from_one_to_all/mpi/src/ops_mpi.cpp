#include "paramonov_from_one_to_all/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <limits>
#include <vector>

#include "paramonov_from_one_to_all/common/include/common.hpp"

namespace paramonov_from_one_to_all {

ParamonovFromOneToAllMPI::ParamonovFromOneToAllMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool ParamonovFromOneToAllMPI::ValidationImpl() {
  const int root = std::get<0>(GetInput());
  const std::vector<int> &buffer = std::get<1>(GetInput());
  // Нужен непустой буфер и корректный номер корневого ранга.
  const bool has_data = !buffer.empty();
  const bool root_non_negative = root >= 0;
  const bool fits_mpi_int = buffer.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max());

  valid_ = has_data && root_non_negative && fits_mpi_int;
  return valid_;
}

bool ParamonovFromOneToAllMPI::PreProcessingImpl() {
  return valid_;
}

bool ParamonovFromOneToAllMPI::RunImpl() {
  if (!valid_) {
    return false;
  }

  const int root = std::get<0>(GetInput());
  const std::vector<int> &buffer = std::get<1>(GetInput());

  int world_size = 0;
  int rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Если root некорректен для данного числа процессов, возвращаем ошибку.
  if (root >= world_size) {
    return false;
  }

  std::vector<int> recv_buffer(buffer.size());

  // root кладет данные в буфер, остальные получают его через MPI_Bcast.
  if (rank == root) {
    recv_buffer = buffer;
  }
  // root рассылает свой буфер, остальные получают его в recv_buffer.
  MPI_Bcast(recv_buffer.data(), static_cast<int>(recv_buffer.size()), MPI_INT, root, MPI_COMM_WORLD);

  GetOutput() = std::move(recv_buffer);
  return true;
}

bool ParamonovFromOneToAllMPI::PostProcessingImpl() {
  return true;
}

}  // namespace paramonov_from_one_to_all
