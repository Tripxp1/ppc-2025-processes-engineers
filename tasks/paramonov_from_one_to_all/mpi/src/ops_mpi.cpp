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

template <typename SendFn, typename RecvFn>
void TreeWalk(int world_size, int logical_rank, const SendFn &send_fn, const RecvFn &recv_fn) {
  for (int step = 1; step < world_size; step <<= 1) {
    if (logical_rank < step && logical_rank + step < world_size) {
      send_fn(step);
    }
    if (logical_rank >= step && logical_rank < 2 * step) {
      recv_fn(step);
    }
  }
}

bool ValidateBroadcastInput(int world_size, int root, std::size_t input_size) {
  if (root < 0 || root >= world_size) {
    return false;
  }
  if (input_size == 0U) {
    return false;
  }
  const int input_count = static_cast<int>(input_size);
  return input_count > 0;
}

void BroadcastCount(int world_size, int logical_rank, int root, int &count) {
  TreeWalk(world_size, logical_rank, [&](int step) {
    const int dst_logical = logical_rank + step;
    const int dst_rank = (dst_logical + root) % world_size;
    MPI_Send(&count, 1, MPI_INT, dst_rank, 0, MPI_COMM_WORLD);
  }, [&](int step) {
    const int src_logical = logical_rank - step;
    const int src_rank = (src_logical + root) % world_size;
    MPI_Recv(&count, 1, MPI_INT, src_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  });
}

template <typename T>
void BroadcastPayload(int world_size, int logical_rank, int root, std::vector<T> &buffer, int count,
                      MPI_Datatype mpi_type) {
  TreeWalk(world_size, logical_rank, [&](int step) {
    const int dst_logical = logical_rank + step;
    const int dst_rank = (dst_logical + root) % world_size;
    MPI_Send(buffer.data(), count, mpi_type, dst_rank, 1, MPI_COMM_WORLD);
  }, [&](int step) {
    const int src_logical = logical_rank - step;
    const int src_rank = (src_logical + root) % world_size;
    MPI_Recv(buffer.data(), count, mpi_type, src_rank, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  });
}

template <typename T>
bool BroadcastVectorTree(const std::vector<T> &input, std::vector<T> &output, int root) {
  int world_size = 0;
  int rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (!ValidateBroadcastInput(world_size, root, input.size())) {
    output.clear();
    return false;
  }

  MPI_Datatype mpi_type = GetMpiType<T>();
  const int input_count = static_cast<int>(input.size());

  // Переводим ранги в "логические", чтобы корень оказался нулём.
  const int logical_rank = (rank - root + world_size) % world_size;

  // Шаг 1: разошлём размер.
  int count = input_count;
  BroadcastCount(world_size, logical_rank, root, count);

  if (count <= 0) {
    output.clear();
    return false;
  }

  output.resize(static_cast<std::size_t>(count));
  if (rank == root) {
    std::copy(input.begin(), input.end(), output.begin());
  }

  // Шаг 2: разошлём сами данные по тому же дереву.
  BroadcastPayload(world_size, logical_rank, root, output, count, mpi_type);

  return true;
}

}  // namespace

ParamonovFromOneToAllMPI::ParamonovFromOneToAllMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = AnyBuffer();
}

bool ParamonovFromOneToAllMPI::ValidationImpl() {
  const int root = std::get<0>(GetInput());
  const AnyBuffer &buffer = std::get<1>(GetInput());
  // Нужен непустой буфер, корректный корень, размер, помещающийся в MPI count, и согласованный тип.
  const bool root_non_negative = root >= 0;
  const bool has_data = !buffer.Empty();
  const bool consistent = buffer.IsConsistent();
  const bool fits_mpi_count = buffer.Size() <= static_cast<std::size_t>(std::numeric_limits<int>::max());

  valid_ = root_non_negative && has_data && consistent && fits_mpi_count;
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

  int world_size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  if (root < 0 || root >= world_size) {
    return false;
  }

  OutType result;
  result.type = input_buffer.type;
  bool ok = false;

  switch (input_buffer.type) {
    case DataType::kInt:
      ok = BroadcastVectorTree(input_buffer.ints, result.ints, root);
      break;
    case DataType::kFloat:
      ok = BroadcastVectorTree(input_buffer.floats, result.floats, root);
      break;
    case DataType::kDouble:
      ok = BroadcastVectorTree(input_buffer.doubles, result.doubles, root);
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
