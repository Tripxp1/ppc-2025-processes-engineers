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
  const bool root_valid = root == 0;
  const bool has_data = !buffer.Empty();
  const bool consistent = buffer.IsConsistent();
  
  valid_ = root_valid && has_data && consistent;
  return valid_;
}

bool ParamonovFromOneToAllSEQ::PreProcessingImpl() {
  return valid_;
}

bool ParamonovFromOneToAllSEQ::RunImpl() {
  if (!valid_) {
    return false;
  }
  GetOutput() = std::get<1>(GetInput());
  return true;
}

bool ParamonovFromOneToAllSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace paramonov_from_one_to_all