#pragma once

#include "paramonov_from_one_to_all/common/include/common.hpp"
#include "task/include/task.hpp"

namespace paramonov_from_one_to_all {

class ParamonovBcastSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit ParamonovBcastSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace paramonov_from_one_to_all