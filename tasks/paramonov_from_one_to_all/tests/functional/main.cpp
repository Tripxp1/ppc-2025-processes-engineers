#include <gtest/gtest.h>
#include <mpi.h>

#include <array>
#include <cstddef>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "paramonov_from_one_to_all/common/include/common.hpp"
#include "paramonov_from_one_to_all/mpi/include/ops_mpi.hpp"
#include "paramonov_from_one_to_all/seq/include/ops_seq.hpp"
#include "task/include/task.hpp"
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
    const std::string &task_name =
        std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kNameTest)>(GetParam());

    const bool is_mpi_launch = ppc::util::IsUnderMpirun();
    if (is_mpi_launch && task_name.find("_seq_") != std::string::npos) {
      GTEST_SKIP();
    }

    const int root = std::get<1>(params);
    if (is_mpi_launch) {
      int world_size = 0;
      MPI_Comm_size(MPI_COMM_WORLD, &world_size);
      if (root >= world_size) {
        GTEST_SKIP();
      }
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
  InType input_data_;
  OutType expected_output_;
};

TEST(BroadcastValidation, RejectsEmptyData) {
  InType input = std::make_tuple(0, MakeIntBuffer({}));
  ParamonovFromOneToAllSEQ seq_task(input);
  EXPECT_FALSE(seq_task.Validation());
}

namespace {

TEST_P(BroadcastFuncTests, BroadcastFromRoot) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 6> kTestParam = {
    std::make_tuple("Int_root0_small", 0, MakeIntBuffer({1, 2, 3, 4})),
    std::make_tuple("Int_root1_negatives", 1, MakeIntBuffer({-5, -1, 0, 7})),
    std::make_tuple("Float_root0", 0, MakeFloatBuffer({1.5F, -2.0F, 3.25F})),
    std::make_tuple("Float_root2", 2, MakeFloatBuffer({0.0F, 5.5F, -1.25F, 8.0F})),
    std::make_tuple("Double_root0", 0, MakeDoubleBuffer({3.14, -6.28, 1.0, 0.5})),
    std::make_tuple("Double_root1", 1, MakeDoubleBuffer({10.0, 20.5, -4.75}))};
const std::vector<TestType> kTestParamVec{kTestParam.begin(), kTestParam.end()};

using ParamType = ppc::util::FuncTestParam<InType, OutType, TestType>;

template <typename Task>
void AppendTasksFor(const std::vector<TestType> &tests, std::vector<ParamType> &out) {
  const std::string type_str =
      ppc::task::GetStringTaskType(Task::GetStaticTypeOfTask(), PPC_SETTINGS_paramonov_from_one_to_all);
  const std::string ns = ppc::util::GetNamespace<Task>();
  for (const auto &t : tests) {
    std::string name = ns;
    name.push_back('_');
    name += type_str;
    out.emplace_back(ppc::task::TaskGetter<Task, InType>, std::move(name), t);
  }
}

std::vector<ParamType> MakeTestParams() {
  std::vector<ParamType> params;

  auto filtered = kTestParamVec;
  if (ppc::util::IsUnderMpirun()) {
    const int world_size = ppc::util::GetNumProc();
    std::erase_if(filtered, [&](const TestType &t) {
      const int root = std::get<1>(t);
      return root >= world_size;
    });
  }

  AppendTasksFor<ParamonovFromOneToAllMPI>(filtered, params);

  if (!ppc::util::IsUnderMpirun()) {
    AppendTasksFor<ParamonovFromOneToAllSEQ>(kTestParamVec, params);
  }

  return params;
}

const auto kTestParamsVec = MakeTestParams();
const auto kGtestValues = ::testing::ValuesIn(kTestParamsVec);

const auto kPerfTestName = BroadcastFuncTests::PrintFuncTestName<BroadcastFuncTests>;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables, modernize-type-traits, misc-use-anonymous-namespace)
INSTANTIATE_TEST_SUITE_P(BroadcastTests, BroadcastFuncTests, kGtestValues, kPerfTestName);
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables, modernize-type-traits, misc-use-anonymous-namespace)

}  // namespace

}  // namespace paramonov_from_one_to_all
