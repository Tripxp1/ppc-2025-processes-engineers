#include "paramonov_from_one_to_all/seq/include/ops_seq.hpp"

#include <vector>

#include "paramonov_from_one_to_all/common/include/common.hpp"

namespace paramonov_from_one_to_all {

ParamonovFromOneToAllSEQ::ParamonovFromOneToAllSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool ParamonovFromOneToAllSEQ::ValidationImpl() {
  const int root = std::get<0>(GetInput());
  const std::vector<int> &buffer = std::get<1>(GetInput());
  // Проверяем, что есть данные и корневой ранг задан неотрицательно.
  const bool has_data = !buffer.empty();
  const bool root_non_negative = root >= 0;
  valid_ = has_data && root_non_negative;
  return valid_;
}

bool ParamonovFromOneToAllSEQ::PreProcessingImpl() {
  return valid_;
}

bool ParamonovFromOneToAllSEQ::RunImpl() {
  if (!valid_) {
    return false;
  }

  // В последовательной версии "рассылка" эквивалентна простому возврату исходного буфера.
  GetOutput() = std::get<1>(GetInput());
  return true;
}

bool ParamonovFromOneToAllSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace paramonov_from_one_to_all
