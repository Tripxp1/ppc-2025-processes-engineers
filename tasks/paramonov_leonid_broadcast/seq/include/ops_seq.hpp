#pragma once
#include <vector>

#include "tasks/paramonov_leonid_broadcast/common/include/common.hpp"
#include "tasks/paramonov_leonid_broadcast/include/task.hpp"

namespace paramonov_leonid_broadcast {

template <typename T>
class ParamonovLeonidBroadcastSEQ : public BaseTask<T> {
 public:
  using InType = std::vector<T>;
  using OutType = std::vector<T>;
  // GCOVR_EXCL_START
  // codecov:disable
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  // GCOVR_EXCL_START
  // codecov:enable
  explicit ParamonovLeonidBroadcastSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace paramonov_leonid_broadcast
