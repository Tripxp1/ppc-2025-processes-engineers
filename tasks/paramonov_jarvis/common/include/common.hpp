#pragma once

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <string>
#include <tuple>
#include <vector>

#include "task/include/task.hpp"

namespace paramonov_jarvis {

struct Point {
  double x{};
  double y{};

  bool operator==(const Point &other) const {
    return x == other.x && y == other.y;
  }
};

using InType = std::vector<Point>;
using OutType = std::vector<Point>;
using TestType = std::tuple<std::string, std::vector<Point>, OutType>;
using BaseTask = ppc::task::Task<InType, OutType>;

namespace detail {

inline double Cross(const Point &a, const Point &b, const Point &c) {
  return ((b.x - a.x) * (c.y - a.y)) - ((b.y - a.y) * (c.x - a.x));
}

inline double Dist2(const Point &a, const Point &b) {
  const double dx = a.x - b.x;
  const double dy = a.y - b.y;
  return (dx * dx) + (dy * dy);
}

inline std::size_t FindLeftmost(const InType &points) {
  std::size_t leftmost = 0;
  for (std::size_t i = 1; i < points.size(); i++) {
    if (points[i].x < points[leftmost].x || (points[i].x == points[leftmost].x && points[i].y < points[leftmost].y)) {
      leftmost = i;
    }
  }
  return leftmost;
}

inline std::size_t SelectNextPoint(const InType &points, std::size_t current, std::size_t candidate) {
  for (std::size_t i = 0; i < points.size(); i++) {
    if (i == current) {
      continue;
    }
    const double cross = Cross(points[current], points[candidate], points[i]);
    const bool better_turn = cross > 0;
    const bool farther_on_line =
        (cross == 0) && (Dist2(points[current], points[i]) > Dist2(points[current], points[candidate]));
    if (better_turn || farther_on_line) {
      candidate = i;
    }
  }
  return candidate;
}

inline void NormalizeHull(OutType &hull) {
  if (hull.size() < 2) {
    return;
  }
  const auto it = std::ranges::min_element(
      hull, [](const Point &a, const Point &b) { return (a.x < b.x) || (a.x == b.x && a.y < b.y); });
  std::ranges::rotate(hull, it);
}

inline OutType BuildHull(const InType &points) {
  if (points.size() < 3) {
    return {};
  }

  OutType hull;
  const std::size_t n = points.size();
  const std::size_t start = FindLeftmost(points);
  std::size_t p = start;
  while (true) {
    hull.push_back(points[p]);
    const std::size_t initial_candidate = (p + 1) % n;
    const std::size_t next = SelectNextPoint(points, p, initial_candidate);
    if (next == start) {
      break;
    }
    if (next == p) {
      break;
    }
    p = next;
  }

  NormalizeHull(hull);
  return hull;
}

}  // namespace detail

}  // namespace paramonov_jarvis
