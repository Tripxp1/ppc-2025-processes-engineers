#pragma once

#include <mpi.h>

#include "paramonov_from_one_to_all/common/include/common.hpp"
#include "task/include/task.hpp"

namespace paramonov_from_one_to_all {

class ParamonovBcastMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit ParamonovBcastMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
  static int MyBcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
};

}  // namespace paramonov_from_one_to_all