#include <gtest/gtest.h>

#include <cstddef>
#include <numeric>
#include <tuple>
#include <utility>
#include <vector>

#include "paramonov_from_one_to_all/common/include/common.hpp"
#include "paramonov_from_one_to_all/mpi/include/ops_mpi.hpp"
#include "paramonov_from_one_to_all/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace paramonov_from_one_to_all {

class BroadcastPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  InType input_data_;
  OutType expected_;

  void SetUp() override {
    const int root = 0;
    const int size = 50000;
    std::vector<int> payload(static_cast<std::size_t>(size));
    std::iota(payload.begin(), payload.end(), -1000);  // NOLINT(modernize-use-ranges)
    expected_ = MakeIntBuffer(std::move(payload));
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

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, ParamonovFromOneToAllMPI, ParamonovFromOneToAllSEQ>(
    PPC_SETTINGS_paramonov_from_one_to_all);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = BroadcastPerfTests::CustomPerfTestName;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables, modernize-type-traits, misc-use-anonymous-namespace)
INSTANTIATE_TEST_SUITE_P(RunModeTests, BroadcastPerfTests, kGtestValues, kPerfTestName);
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables, modernize-type-traits, misc-use-anonymous-namespace)

}  // namespace paramonov_from_one_to_all
