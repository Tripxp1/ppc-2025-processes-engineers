#pragma once

#include <vector>

#include "paramonov_from_one_to_all/common/include/common.hpp"
#include "task/include/task.hpp"

namespace paramonov_from_one_to_all {

class ParamonovFromOneToAllProhodSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit ParamonovFromOneToAllProhodSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  static std::vector<Point> JarvisMarch(std::vector<Point> points);
};

}  // namespace paramonov_from_one_to_all
