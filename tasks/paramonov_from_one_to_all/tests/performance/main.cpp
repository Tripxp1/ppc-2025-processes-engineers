#include <gtest/gtest.h>

#include <numeric>
#include <vector>

#include "paramonov_from_one_to_all/common/include/common.hpp"
#include "paramonov_from_one_to_all/mpi/include/ops_mpi.hpp"
#include "paramonov_from_one_to_all/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace paramonov_from_one_to_all {

class BroadcastPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  InType input_data_{};
  OutType expected_{};

  void SetUp() override {
    const int root = 0;
    const int size = 50000;
    expected_.resize(size);
    std::iota(expected_.begin(), expected_.end(), -1000);
    input_data_ = std::make_tuple(root, expected_);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == expected_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(BroadcastPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, ParamonovFromOneToAllMPI, ParamonovFromOneToAllSEQ>(
        PPC_SETTINGS_paramonov_from_one_to_all);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = BroadcastPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, BroadcastPerfTests, kGtestValues, kPerfTestName);

}  // namespace paramonov_from_one_to_all
