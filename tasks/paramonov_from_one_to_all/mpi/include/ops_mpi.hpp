#pragma once

#include <mpi.h>

#include "paramonov_from_one_to_all/common/include/common.hpp"
#include "task/include/task.hpp"

namespace paramonov_from_one_to_all {

int ParamonovFromOneToAllBroadcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);

class ParamonovFromOneToAllBroadcastMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit ParamonovFromOneToAllBroadcastMPI(const InType &in);

 private:
  template <typename T>
  bool ProcessVector(const InType &input_data, int proc_rank, int root_proc, MPI_Datatype mpi_dtype);

  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace paramonov_from_one_to_all
