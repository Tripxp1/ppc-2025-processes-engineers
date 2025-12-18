#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <string>
#include <tuple>

#include "tasks/paramonov_leonid_broadcast/common/include/common.hpp"
#include "tasks/paramonov_leonid_broadcast/mpi/include/ops_mpi.hpp"
#include "tasks/paramonov_leonid_broadcast/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace paramonov_leonid_broadcast {

class ParamonovLeonidRunFuncTestsProcessesBR : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int size = std::get<0>(params);

    input_data_.resize(size);
    for (int i = 0; i < size; ++i) {
      input_data_[i] = (i * 187345543) % 100;
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return std::equal(input_data_.begin(), input_data_.end(), output_data.begin());
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
};

namespace {

TEST_P(ParamonovLeonidRunFuncTestsProcessesBR, MatmulFromPic) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 3> kTestParam = {std::make_tuple(3, "3"), std::make_tuple(5, "5"), std::make_tuple(7, "7")};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<ParamonovLeonidBroadcastMPI<int>, InType>(
                                               kTestParam, PPC_SETTINGS_paramonov_leonid_broadcast),
                                           ppc::util::AddFuncTask<ParamonovLeonidBroadcastSEQ<int>, InType>(
                                               kTestParam, PPC_SETTINGS_paramonov_leonid_broadcast));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    ParamonovLeonidRunFuncTestsProcessesBR::PrintFuncTestName<ParamonovLeonidRunFuncTestsProcessesBR>;

INSTANTIATE_TEST_SUITE_P(PicMatrixTests, ParamonovLeonidRunFuncTestsProcessesBR, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace paramonov_leonid_broadcast
