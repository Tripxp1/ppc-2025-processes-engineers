#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "task/include/task.hpp"

namespace paramonov_from_one_to_all {

enum class DataType : std::uint8_t { kInt, kFloat, kDouble };

struct AnyBuffer {
  DataType type{DataType::kInt};
  std::vector<int> ints;
  std::vector<float> floats;
  std::vector<double> doubles;

  template <typename T>
  [[nodiscard]] const std::vector<T> &Get() const;

  template <typename T>
  [[nodiscard]] std::vector<T> &Get();

  [[nodiscard]] std::size_t Size() const;
  [[nodiscard]] bool Empty() const {
    return Size() == 0;
  }
  [[nodiscard]] bool IsConsistent() const;
};

inline bool operator==(const AnyBuffer &lhs, const AnyBuffer &rhs) {
  if (lhs.type != rhs.type) {
    return false;
  }
  switch (lhs.type) {
    case DataType::kInt:
      return lhs.ints == rhs.ints;
    case DataType::kFloat:
      return lhs.floats == rhs.floats;
    case DataType::kDouble:
      return lhs.doubles == rhs.doubles;
    default:
      return false;
  }
}

inline AnyBuffer MakeIntBuffer(std::vector<int> data) {
  AnyBuffer buf;
  buf.type = DataType::kInt;
  buf.ints = std::move(data);
  return buf;
}

inline AnyBuffer MakeFloatBuffer(std::vector<float> data) {
  AnyBuffer buf;
  buf.type = DataType::kFloat;
  buf.floats = std::move(data);
  return buf;
}

inline AnyBuffer MakeDoubleBuffer(std::vector<double> data) {
  AnyBuffer buf;
  buf.type = DataType::kDouble;
  buf.doubles = std::move(data);
  return buf;
}

inline const char *ToString(DataType type) {
  switch (type) {
    case DataType::kInt:
      return "int";
    case DataType::kFloat:
      return "float";
    case DataType::kDouble:
      return "double";
    default:
      return "unknown";
  }
}

inline void PrintTo(const AnyBuffer &buf, std::ostream *os) {
  *os << "AnyBuffer(type=" << ToString(buf.type) << ", size=" << buf.Size() << ", data=[";
  bool first = true;
  auto print_vec = [&](const auto &vec) {
    for (const auto &v : vec) {
      if (!first) {
        *os << ",";
      }
      first = false;
      *os << v;
    }
  };
  switch (buf.type) {
    case DataType::kInt:
      print_vec(buf.ints);
      break;
    case DataType::kFloat:
      print_vec(buf.floats);
      break;
    case DataType::kDouble:
      print_vec(buf.doubles);
      break;
    default:
      *os << "unknown_type";
  }
  *os << "])";
}

inline std::ostream &operator<<(std::ostream &os, const AnyBuffer &buf) {
  PrintTo(buf, &os);
  return os;
}

using InType = std::tuple<int, AnyBuffer>;
using OutType = AnyBuffer;
using TestType = std::tuple<std::string, int, AnyBuffer>;
using BaseTask = ppc::task::Task<InType, OutType>;

template <>
inline const std::vector<int> &AnyBuffer::Get<int>() const {
  return ints;
}

template <>
inline const std::vector<float> &AnyBuffer::Get<float>() const {
  return floats;
}

template <>
inline const std::vector<double> &AnyBuffer::Get<double>() const {
  return doubles;
}

template <>
inline std::vector<int> &AnyBuffer::Get<int>() {
  return ints;
}

template <>
inline std::vector<float> &AnyBuffer::Get<float>() {
  return floats;
}

template <>
inline std::vector<double> &AnyBuffer::Get<double>() {
  return doubles;
}

inline std::size_t AnyBuffer::Size() const {
  switch (type) {
    case DataType::kInt:
      return ints.size();
    case DataType::kFloat:
      return floats.size();
    case DataType::kDouble:
      return doubles.size();
    default:
      return 0;
  }
}

inline bool AnyBuffer::IsConsistent() const {
  switch (type) {
    case DataType::kInt:
      return !ints.empty() && floats.empty() && doubles.empty();
    case DataType::kFloat:
      return !floats.empty() && ints.empty() && doubles.empty();
    case DataType::kDouble:
      return !doubles.empty() && ints.empty() && floats.empty();
    default:
      return false;
  }
}

}  // namespace paramonov_from_one_to_all