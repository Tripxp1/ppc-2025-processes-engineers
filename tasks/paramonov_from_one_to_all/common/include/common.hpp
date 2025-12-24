#pragma once

#include <tuple>
#include <vector>

#include "task/include/task.hpp"

namespace paramonov_from_one_to_all {

using InType = std::tuple<int, int, std::vector<int>, std::vector<float>, std::vector<double>>;
using OutType = bool;
using TestType = std::tuple<int, int, bool>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace paramonov_from_one_to_all