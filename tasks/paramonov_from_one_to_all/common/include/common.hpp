#pragma once

#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "task/include/task.hpp"

namespace paramonov_from_one_to_all {

using InTypeVariant = std::variant<std::vector<int>, std::vector<float>, std::vector<double>>;

struct InType {
  InTypeVariant data;
  int root{0};
};

using OutType = InTypeVariant;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace paramonov_from_one_to_all
