#pragma once

#include <vector>
#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace paramonov_from_one_to_all {

using InType = std::tuple<int, std::vector<int>>;
using OutType = std::vector<int>;
using TestType = std::tuple<std::string, int, std::vector<int>>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace paramonov_from_one_to_all
