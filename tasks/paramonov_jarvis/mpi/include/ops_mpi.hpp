#pragma once

#include <vector>

#include "paramonov_from_one_to_all/common/include/common.hpp"
#include "task/include/task.hpp"

namespace paramonov_from_one_to_all {

class ParamonovFromOneToAllProhodMPI : public BaseTask {
public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() noexcept {
    return ppc::task::TypeOfTask::kMPI;
  }

  explicit ParamonovFromOneToAllProhodMPI(const InType &in);

  static std::vector<Point> JarvisMarch(std::vector<Point> points);

private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  static std::vector<Point> FinalHull(int rank,
                                      std::vector<Point> &all_hull_points);
};

} // namespace paramonov_from_one_to_all