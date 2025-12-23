#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <tuple>

#include "paramonov_jarvis/common/include/common.hpp"
#include "paramonov_jarvis/mpi/include/ops_mpi.hpp"
#include "paramonov_jarvis/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace paramonov_jarvis {

class JarvisFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::get<0>(test_param);
  }

 protected:
  void SetUp() override {
    const TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    input_data_ = std::get<1>(params);
    expected_output_ = std::get<2>(params);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if (output_data.size() != expected_output_.size()) {
      return false;
    }

    auto matches_with_rotation = [&](const OutType &cand) {
      const std::size_t n = cand.size();
      for (std::size_t shift = 0; shift < n; shift++) {
        bool ok = true;
        for (std::size_t i = 0; i < n; i++) {
          const Point &a = cand[(i + shift) % n];
          const Point &b = expected_output_[i];
          if (!(a == b)) {
            ok = false;
            break;
          }
        }
        if (ok) {
          return true;
        }
      }
      return false;
    };

    // Сравниваем с учётом возможного циклического сдвига и ориентации.
    if (matches_with_rotation(output_data)) {
      return true;
    }
    OutType reversed = output_data;
    std::ranges::reverse(reversed);
    return matches_with_rotation(reversed);
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  OutType expected_output_;
};

TEST(JarvisValidation, RejectsSmallInput) {
  InType too_small = {{0.0, 0.0}, {1.0, 1.0}};
  ParamonovJarvisSEQ task(too_small);
  EXPECT_FALSE(task.Validation());
}

namespace {

TEST_P(JarvisFuncTests, ComputesConvexHull) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 4> kTestParam = {
    std::make_tuple("Square_with_inner", InType{{0, 0}, {2, 0}, {2, 2}, {0, 2}, {1, 1}},
                    OutType{{0, 0}, {0, 2}, {2, 2}, {2, 0}}),
    std::make_tuple("Triangle_with_inside", InType{{0, 0}, {3, 0}, {1.5, 2.5}, {1, 1}},
                    OutType{{0, 0}, {3, 0}, {1.5, 2.5}}),
    std::make_tuple("Concave_L_shape", InType{{0, 0}, {2, 0}, {2, 1}, {1, 1}, {1, 2}, {0, 2}},
                    OutType{{0, 0}, {2, 0}, {2, 1}, {1, 2}, {0, 2}}),
    std::make_tuple("Collinear_points", InType{{0, 0}, {1, 0}, {3, 0}, {2, 0}}, OutType{{0, 0}, {3, 0}})};

const auto kTestTasksList =
    std::tuple_cat(ppc::util::AddFuncTask<ParamonovJarvisMPI, InType>(kTestParam, PPC_SETTINGS_paramonov_jarvis),
                   ppc::util::AddFuncTask<ParamonovJarvisSEQ, InType>(kTestParam, PPC_SETTINGS_paramonov_jarvis));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = JarvisFuncTests::PrintFuncTestName<JarvisFuncTests>;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables, modernize-type-traits, misc-use-anonymous-namespace)
INSTANTIATE_TEST_SUITE_P(ConvexHullTests, JarvisFuncTests, kGtestValues, kPerfTestName);
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables, modernize-type-traits, misc-use-anonymous-namespace)

}  // namespace

}  // namespace paramonov_jarvis
