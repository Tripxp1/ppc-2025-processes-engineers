#include <gtest/gtest.h>

#include <tuple>
#include <vector>

#include "paramonov_from_one_to_all/common/include/common.hpp"
#include "paramonov_from_one_to_all/mpi/include/ops_mpi.hpp"
#include "paramonov_from_one_to_all/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace paramonov_from_one_to_all {

class ParamonovBcastPerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
 private:
  InType input_data_;
  double count_ = 1000000;

 public:
  void SetUp() override {
    std::vector<int> v_int(0);
    std::vector<float> v_float(0);
    std::vector<double> v_double(0);
    for (int i = 0; i < count_; i++) {
      v_int.push_back(i);
      v_float.push_back(static_cast<float>(i));
      v_double.push_back(static_cast<double>(i));
    }
    int main_proc = 1;
    input_data_ = std::tuple<int, int, std::vector<int>, std::vector<float>, std::vector<double>>(
        main_proc, count_, v_int, v_float, v_double);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ParamonovBcastPerfTest, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, ParamonovBcastMPI, ParamonovBcastSEQ>(PPC_SETTINGS_paramonov_from_one_to_all);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ParamonovBcastPerfTest::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(BcastPerfTests, ParamonovBcastPerfTest, kGtestValues, kPerfTestName);

}  // namespace paramonov_from_one_to_all