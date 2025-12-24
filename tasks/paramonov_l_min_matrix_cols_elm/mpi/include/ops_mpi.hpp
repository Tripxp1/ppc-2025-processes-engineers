#pragma once

#include <utility>
#include <vector>

#include "Paramonov_L_Min_Matrix_Cols_Elm/common/include/common.hpp"
#include "task/include/task.hpp"

namespace paramonov_l_min_matrix_cols_elm {

class ParamonovLMinMatrixColsElmMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit ParamonovLMinMatrixColsElmMPI(const InType &in);

  static std::vector<int> FlattenMatrix(const std::vector<std::vector<int>> &matrix);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  static std::pair<int, int> PrepareDimensions(const std::vector<std::vector<int>> &matrix, int rank);
};

}  // namespace paramonov_l_min_matrix_cols_elm
