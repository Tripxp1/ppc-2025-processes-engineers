#include <gtest/gtest.h>

#include <array>
#include <string>
#include <tuple>
#include <vector>

#include "paramonov_from_one_to_all/common/include/common.hpp"
#include "paramonov_from_one_to_all/mpi/include/ops_mpi.hpp"
#include "paramonov_from_one_to_all/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace paramonov_from_one_to_all {

class BroadcastFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::get<0>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    const int root = std::get<1>(params);
    if (ppc::util::IsUnderMpirun() && root >= ppc::util::GetNumProc()) {
      GTEST_SKIP();  // Нет нужного числа процессов для выбранного root.
    }
    input_data_ = std::make_tuple(std::get<1>(params), std::get<2>(params));
    expected_output_ = std::get<2>(params);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == expected_output_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_{};
  OutType expected_output_{};
};

TEST(BroadcastValidation, RejectsEmptyData) {
  InType input = std::make_tuple(0, std::vector<int>{});
  ParamonovFromOneToAllSEQ seq_task(input);
  EXPECT_FALSE(seq_task.Validation());
}

namespace {

TEST_P(BroadcastFuncTests, BroadcastFromRoot) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 4> kTestParam = {
    std::make_tuple("Root0_small", 0, std::vector<int>{1, 2, 3, 4}),
    std::make_tuple("Root1_negatives", 1, std::vector<int>{-5, -1, 0, 7}),
    std::make_tuple("Root0_single_value", 0, std::vector<int>{42}),
    std::make_tuple("Root2_mixed", 2, std::vector<int>{9, 8, -3, 15, 0, 2})};

const auto kTestTasksList =
    std::tuple_cat(ppc::util::AddFuncTask<ParamonovFromOneToAllMPI, InType>(kTestParam, PPC_SETTINGS_paramonov_from_one_to_all),
                   ppc::util::AddFuncTask<ParamonovFromOneToAllSEQ, InType>(kTestParam, PPC_SETTINGS_paramonov_from_one_to_all));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = BroadcastFuncTests::PrintFuncTestName<BroadcastFuncTests>;

INSTANTIATE_TEST_SUITE_P(BroadcastTests, BroadcastFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace paramonov_from_one_to_all
