#include "paramonov_jarvis/seq/include/ops_seq.hpp"

#include <vector>

#include "paramonov_jarvis/common/include/common.hpp"

namespace paramonov_jarvis {

ParamonovJarvisSEQ::ParamonovJarvisSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = OutType();
}

bool ParamonovJarvisSEQ::ValidationImpl() {
  const auto &points = GetInput();
  valid_ = points.size() >= 3;
  return valid_;
}

bool ParamonovJarvisSEQ::PreProcessingImpl() {
  return valid_;
}

bool ParamonovJarvisSEQ::RunImpl() {
  if (!valid_) {
    return false;
  }

  GetOutput() = detail::BuildHull(GetInput());
  return true;
}

bool ParamonovJarvisSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace paramonov_jarvis
