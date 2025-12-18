#include <gtest/gtest.h>

#include <algorithm>

#include "tasks/paramonov_leonid_broadcast/common/include/common.hpp"
#include "tasks/paramonov_leonid_broadcast/mpi/include/ops_mpi.hpp"
#include "tasks/paramonov_leonid_broadcast/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace paramonov_leonid_broadcast {

class ParamonovLeonidRunPerfTestProcessesBR : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 10000000;
  InType input_data_;

  void SetUp() override {
    input_data_.resize(kCount_, 7863453);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return std::equal(input_data_.begin(), input_data_.end(), output_data.begin());
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ParamonovLeonidRunPerfTestProcessesBR, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, ParamonovLeonidBroadcastMPI<int>, ParamonovLeonidBroadcastSEQ<int>>(
        PPC_SETTINGS_paramonov_leonid_broadcast);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ParamonovLeonidRunPerfTestProcessesBR::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ParamonovLeonidRunPerfTestProcessesBR, kGtestValues, kPerfTestName);

}  // namespace paramonov_leonid_broadcast
