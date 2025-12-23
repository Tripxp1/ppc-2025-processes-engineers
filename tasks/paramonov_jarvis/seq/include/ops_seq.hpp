#pragma once

#include "paramonov_jarvis/common/include/common.hpp"
#include "task/include/task.hpp"

namespace paramonov_jarvis {

class ParamonovJarvisSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit ParamonovJarvisSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  bool valid_ = false;
};

}  // namespace paramonov_jarvis
