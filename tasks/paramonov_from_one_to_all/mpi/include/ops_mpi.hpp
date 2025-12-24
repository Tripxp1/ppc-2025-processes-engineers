#include "paramonov_from_one_to_all/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "paramonov_from_one_to_all/common/include/common.hpp"

namespace paramonov_from_one_to_all {

namespace {

template <typename T>
MPI_Datatype GetMpiType();

template <>
MPI_Datatype GetMpiType<int>() {
  return MPI_INT;
}

template <>
MPI_Datatype GetMpiType<float>() {
  return MPI_FLOAT;
}

template <>
MPI_Datatype GetMpiType<double>() {
  return MPI_DOUBLE;
}

template <typename T>
bool BroadcastVector(const std::vector<T> &input, std::vector<T> &output, int root) {
  int world_size = 0;
  int rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  if (root < 0 || root >= world_size) {
    return false;
  }
  
  if (input.empty()) {
    return false;
  }
  
  MPI_Datatype mpi_type = GetMpiType<T>();
  
  int count = static_cast<int>(input.size());
  MPI_Bcast(&count, 1, MPI_INT, root, MPI_COMM_WORLD);
  
  if (count <= 0) {
    return false;
  }
  
  output.resize(static_cast<std::size_t>(count));
  
  if (rank == root) {
    std::copy(input.begin(), input.end(), output.begin());
  }
  
  MPI_Bcast(output.data(), count, mpi_type, root, MPI_COMM_WORLD);
  
  return true;
}

}  

ParamonovFromOneToAllMPI::ParamonovFromOneToAllMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = AnyBuffer();
}

bool ParamonovFromOneToAllMPI::ValidationImpl() {
  const int root = std::get<0>(GetInput());
  const AnyBuffer &buffer = std::get<1>(GetInput());
  
  int world_size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  
  const bool root_valid = root >= 0 && root < world_size;
  const bool has_data = !buffer.Empty();
  const bool consistent = buffer.IsConsistent();
  const bool fits_mpi_count = buffer.Size() <= static_cast<std::size_t>(std::numeric_limits<int>::max());
  
  valid_ = root_valid && has_data && consistent && fits_mpi_count;
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
  const AnyBuffer &input_buffer = std::get<1>(GetInput());
  
  OutType result;
  result.type = input_buffer.type;
  bool ok = false;
  
  switch (input_buffer.type) {
    case DataType::kInt:
      ok = BroadcastVector(input_buffer.ints, result.ints, root);
      break;
    case DataType::kFloat:
      ok = BroadcastVector(input_buffer.floats, result.floats, root);
      break;
    case DataType::kDouble:
      ok = BroadcastVector(input_buffer.doubles, result.doubles, root);
      break;
  }
  
  if (!ok) {
    return false;
  }
  
  GetOutput() = std::move(result);
  return true;
}

bool ParamonovFromOneToAllMPI::PostProcessingImpl() {
  return true;
}

}  // namespace paramonov_from_one_to_all