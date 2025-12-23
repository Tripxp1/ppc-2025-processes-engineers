#include "paramonov_jarvis/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cstddef>
#include <vector>

#include "paramonov_jarvis/common/include/common.hpp"

namespace paramonov_jarvis {

ParamonovJarvisMPI::ParamonovJarvisMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = OutType();
}

bool ParamonovJarvisMPI::ValidationImpl() {
  valid_ = GetInput().size() >= 3;
  return valid_;
}

bool ParamonovJarvisMPI::PreProcessingImpl() {
  return valid_;
}

bool ParamonovJarvisMPI::RunImpl() {
  if (!valid_) {
    return false;
  }

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Простая схема: корневой процесс строит оболочку и рассылает ответ.
  std::vector<Point> hull;
  if (rank == 0) {
    hull = detail::BuildHull(GetInput());
  }

  // Рассылаем размер результата.
  int hull_size = static_cast<int>(hull.size());
  MPI_Bcast(&hull_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (hull_size < 0) {
    return false;
  }

  // Рассылаем точки как массив из x и y.
  std::vector<double> packed;
  if (rank == 0) {
    packed.reserve(static_cast<std::size_t>(hull_size) * 2);
    for (const auto &p : hull) {
      packed.push_back(p.x);
      packed.push_back(p.y);
    }
  } else {
    packed.resize(static_cast<std::size_t>(hull_size) * 2);
  }
  MPI_Bcast(packed.data(), hull_size * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (rank != 0) {
    hull.resize(static_cast<std::size_t>(hull_size));
    for (int i = 0; i < hull_size; i++) {
      const std::size_t base = static_cast<std::size_t>(i) * 2;
      hull[static_cast<std::size_t>(i)].x = packed[base];
      hull[static_cast<std::size_t>(i)].y = packed[base + 1];
    }
  }

  GetOutput() = hull;
  return true;
}

bool ParamonovJarvisMPI::PostProcessingImpl() {
  return true;
}

}  // namespace paramonov_jarvis
