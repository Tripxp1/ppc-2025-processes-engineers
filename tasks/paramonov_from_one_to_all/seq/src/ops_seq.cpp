#include "paramonov_from_one_to_all/seq/include/ops_seq.hpp"

#include "paramonov_from_one_to_all/common/include/common.hpp"

namespace paramonov_from_one_to_all {

ParamonovBcastSEQ::ParamonovBcastSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = false;
}

bool ParamonovBcastSEQ::ValidationImpl() {
  return true;
}

bool ParamonovBcastSEQ::PreProcessingImpl() {
  return true;
}

bool ParamonovBcastSEQ::RunImpl() {
  GetOutput() = true;
  return true;
}

bool ParamonovBcastSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace paramonov_from_one_to_all