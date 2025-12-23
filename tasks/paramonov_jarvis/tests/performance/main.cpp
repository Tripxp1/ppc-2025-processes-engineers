#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

#include "paramonov_jarvis/common/include/common.hpp"
#include "paramonov_jarvis/mpi/include/ops_mpi.hpp"
#include "paramonov_jarvis/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace paramonov_jarvis {

class JarvisPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  InType input_data_;
  OutType expected_;

  void SetUp() override {
    const std::size_t count = 20000;
    std::uint32_t state = 0x12345678U;
    auto next_val = [&]() {
      state = (state * 1664525U) + 1013904223U;  // простой LCG
      return state;
    };
    input_data_.resize(count);
    for (auto &pt : input_data_) {
      const auto vx = next_val();
      const auto vy = next_val();
      pt.x = static_cast<double>(static_cast<int32_t>(vx % 20001U) - 10000);
      pt.y = static_cast<double>(static_cast<int32_t>(vy % 20001U) - 10000);
    }
    expected_ = detail::BuildHull(input_data_);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == expected_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(JarvisPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, ParamonovJarvisMPI, ParamonovJarvisSEQ>(PPC_SETTINGS_paramonov_jarvis);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = JarvisPerfTests::CustomPerfTestName;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables, modernize-type-traits, misc-use-anonymous-namespace)
INSTANTIATE_TEST_SUITE_P(RunModeTests, JarvisPerfTests, kGtestValues, kPerfTestName);
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables, modernize-type-traits, misc-use-anonymous-namespace)

}  // namespace paramonov_jarvis
