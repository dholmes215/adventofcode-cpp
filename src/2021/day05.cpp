//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <ctre.hpp>

// static_assert(__cplusplus >= 201703L);
// #ifdef _MSC_VER
// #pragma warning(push)
// #pragma warning(disable : 4242)
// #pragma warning(disable : 4244)
// #pragma warning(disable : 4324)
// #endif
// #include <absl/container/btree_map.h>
// #ifdef _MSC_VER
// #pragma warning(pop)
// #endif

#include <map>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

namespace {

}  // namespace

using point = vec2<int>;

struct vent_line {
    point a{};
    point b{};
};

vent_line parse_line(std::string_view line)
{
    constexpr auto vent_matcher{ctre::match<"(\\d+),(\\d+) -> (\\d+),(\\d+)">};
    if (auto [whole, x1, y1, x2, y2] = vent_matcher(line); whole) {
        return vent_line{vec2<int>{to_int(x1.to_view()), to_int(y1.to_view())},
                         vec2<int>{to_int(x2.to_view()), to_int(y2.to_view())}};
    }
    throw input_error{fmt::format("failed to parse line: {}", line)};
}

bool line_is_horiz_or_vert(const vent_line& line)
{
    return line.a.x == line.b.x || line.a.y == line.b.y;
}

std::vector<point> line_points(const vent_line& line)
{
    std::vector<point> out;

    point step{line.b - line.a};
    if (step.x != 0) {
        step.x /= std::abs(step.x);
    }
    if (step.y != 0) {
        step.y /= std::abs(step.y);
    }

    point p{line.a};
    out.push_back(p);
    while (p != line.b) {
        p += step;
        out.push_back(p);
    }
    return out;
}

std::vector<vent_line> parse_lines(std::string_view input)
{
    std::vector<vent_line> out;
    for (std::string_view l : sv_lines(input)) {
        out.emplace_back(parse_line(l));
    }
    return out;
}

auto solve(const auto& lines)
{
    // absl::btree_map<point, int> point_counts;
    std::map<point, int> point_counts;
    for (const auto& vl : lines) {
        for (const auto p : line_points(vl)) {
            point_counts[p]++;
        }
    }

    return r::count_if(point_counts | rv::values, [](int i) { return i > 1; });
}

aoc::solution_result day05(std::string_view input)
{
    const std::vector<vent_line> all_vent_lines{parse_lines(input)};
    const auto horiz_vert_vent_lines{all_vent_lines |
                                     rv::filter(line_is_horiz_or_vert) |
                                     r::to<std::vector>};

    const auto count_a{solve(horiz_vert_vent_lines)};
    const auto count_b{solve(all_vent_lines)};
    return {count_a, count_b};
}

}  // namespace aoc::year2021
