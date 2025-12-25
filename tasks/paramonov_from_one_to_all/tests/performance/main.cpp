#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <variant>
#include <vector>

#include "paramonov_from_one_to_all/common/include/common.hpp"
#include "paramonov_from_one_to_all/mpi/include/ops_mpi.hpp"
#include "paramonov_from_one_to_all/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace paramonov_from_one_to_all {

class ParamonovFromOneToAllRunPerfTestProcesses : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kArraySize_ = 6000000;
  InType test_input_{};

  void SetUp() override {
    std::vector<double> test_data(kArraySize_);
    for (int idx = 0; idx < kArraySize_; ++idx) {
      test_data[idx] = static_cast<double>(idx) * 0.75;
    }
    test_input_ = InType{.data = InTypeVariant{test_data}, .root = 0};
  }

  bool CheckTestOutputData(OutType &result) final {
    if (!std::holds_alternative<std::vector<double>>(test_input_.data)) {
      return false;
    }
    const auto &src_data = std::get<std::vector<double>>(test_input_.data);
    if (!std::holds_alternative<std::vector<double>>(result)) {
      return false;
    }
    const auto &dst_data = std::get<std::vector<double>>(result);
    if (dst_data.size() != src_data.size()) {
      return false;
    }
    const double tolerance = 1e-10;
    for (std::size_t idx = 0; idx < dst_data.size(); ++idx) {
      if (std::fabs(dst_data[idx] - src_data[idx]) > tolerance) {
        return false;
      }
    }
    return true;
  }

  InType GetTestInputData() final {
    return test_input_;
  }
};

TEST_P(ParamonovFromOneToAllRunPerfTestProcesses, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, ParamonovFromOneToAllBroadcastMPI, ParamonovFromOneToAllBroadcastSEQ>(
        PPC_SETTINGS_paramonov_from_one_to_all);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ParamonovFromOneToAllRunPerfTestProcesses::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ParamonovFromOneToAllRunPerfTestProcesses, kGtestValues, kPerfTestName);

}  // namespace paramonov_from_one_to_all
