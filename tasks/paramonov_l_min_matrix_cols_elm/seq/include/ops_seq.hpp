#pragma once

#include "Paramonov_L_Min_Matrix_Cols_Elm/common/include/common.hpp"
#include "task/include/task.hpp"

namespace paramonov_l_min_matrix_cols_elm {

class ParamonovLMinMatrixColsElmSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit ParamonovLMinMatrixColsElmSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace paramonov_l_min_matrix_cols_elm
