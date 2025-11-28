#include "paramonov_l_min_matrix_cols_elm/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

#include "paramonov_l_min_matrix_cols_elm/common/include/common.hpp"
#include "util/include/util.hpp"

namespace paramonov_l_min_matrix_cols_elm {

ParamonovLMinMatrixColsElmSEQ::ParamonovLMinMatrixColsElmSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool ParamonovLMinMatrixColsElmSEQ::ValidationImpl() {
  std::size_t m = std::get<0>(GetInput());
  std::size_t n = std::get<1>(GetInput());
  std::vector<int> &val = std::get<2>(GetInput());
  valid_ = (n > 0) && (m > 0) && (val.size() == (n * m));
  return valid_;
}

bool ParamonovLMinMatrixColsElmSEQ::PreProcessingImpl() {
  if (valid_) {
    t_matrix_ = std::get<2>(GetInput()); 
    return true;
  }
  return false;
}

bool ParamonovLMinMatrixColsElmSEQ::RunImpl() {
  if (!valid_) {
    return false;
  }
  
  std::size_t m = std::get<0>(GetInput());
  std::size_t n = std::get<1>(GetInput());

  std::vector<int> min_cols_elm(m); 
  
  for (std::size_t j = 0; j < m; j++) {      
    min_cols_elm[j] = t_matrix_[j];          
    for (std::size_t i = 1; i < n; i++) {    
      min_cols_elm[j] = std::min(min_cols_elm[j], t_matrix_[(i * m) + j]);
    }
  }
  
  GetOutput() = min_cols_elm;
  return true;
}

bool ParamonovLMinMatrixColsElmSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace paramonov_l_min_matrix_cols_elm