#include "paramonov_from_one_to_all/seq/include/ops_seq.hpp"

#include "paramonov_from_one_to_all/common/include/common.hpp"

namespace paramonov_from_one_to_all {

ParamonovFromOneToAllSEQ::ParamonovFromOneToAllSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = AnyBuffer();
}

bool ParamonovFromOneToAllSEQ::ValidationImpl() {
  const int root = std::get<0>(GetInput());
  const AnyBuffer &buffer = std::get<1>(GetInput());
  // Проверяем, что буфер непустой, корневой ранг корректный и тип/данные согласованы.
  const bool has_data = !buffer.Empty();
  const bool root_non_negative = root >= 0;
  const bool consistent = buffer.IsConsistent();
  valid_ = has_data && root_non_negative && consistent;
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
