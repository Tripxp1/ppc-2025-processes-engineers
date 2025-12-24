#include "Paramonov_L_Min_Matrix_Cols_Elm/seq/include/ops_seq.hpp"

#include <algorithm>
#include <climits>
#include <cstddef>
#include <ranges>  // IWYU pragma: keep
#include <vector>

#include "Paramonov_L_Min_Matrix_Cols_Elm/common/include/common.hpp"

namespace paramonov_l_min_matrix_cols_elm {

ParamonovLMinMatrixColsElmSEQ::ParamonovLMinMatrixColsElmSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());

  if (!in.empty()) {
    GetInput() = in;
  } else {
    GetInput() = InType{};
  }

  GetOutput() = OutType{};
}

bool ParamonovLMinMatrixColsElmSEQ::ValidationImpl() {
  const auto &input = GetInput();
  if (input.empty()) {
    return false;
  }

  const std::size_t cols = input[0].size();
  if (cols == 0) {
    return false;
  }

  return std::ranges::all_of(input, [cols](const auto &row) { return row.size() == cols; });
}

bool ParamonovLMinMatrixColsElmSEQ::PreProcessingImpl() {
  GetOutput().clear();
  return true;
}

bool ParamonovLMinMatrixColsElmSEQ::RunImpl() {
  const auto &matrix = GetInput();
  auto &result = GetOutput();

  result.clear();

  if (matrix.empty()) {
    return false;
  }

  const std::size_t rows = matrix.size();
  const std::size_t cols = matrix[0].size();

  result.assign(cols, INT_MAX);

  for (std::size_t i = 0; i < rows; ++i) {
    for (std::size_t j = 0; j < cols; ++j) {
      const int val = matrix[i][j];
      result[j] = std::min(val, result[j]);
    }
  }

  return true;
}

bool ParamonovLMinMatrixColsElmSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace paramonov_l_min_matrix_cols_elm