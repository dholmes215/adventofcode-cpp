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

#include <cstdint>
#include <map>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

namespace {

}  // namespace

using scalar = std::int16_t;
using point = vec2<scalar>;

scalar to_scalar(std::string_view s)
{
    return to_num<scalar>(s);
}

struct vent_line {
    point a{};
    point b{};
};

vent_line parse_line(std::string_view line)
{
    constexpr auto vent_matcher{ctre::match<"(\\d+),(\\d+) -> (\\d+),(\\d+)">};
    if (auto [whole, x1, y1, x2, y2] = vent_matcher(line); whole) {
        return vent_line{
            point{to_scalar(x1.to_view()), to_scalar(y1.to_view())},
            point{to_scalar(x2.to_view()), to_scalar(y2.to_view())}};
    }
    throw input_error{fmt::format("failed to parse line: {}", line)};
}

std::vector<vent_line> parse_lines(std::string_view input)
{
    std::vector<vent_line> out;
    out.resize(500);
    r::copy(sv_lines(input) | rv::transform(parse_line),
            r::back_inserter(out));
    return out;
}

bool line_is_horiz_or_vert(const vent_line& line)
{
    return line.a.x == line.b.x || line.a.y == line.b.y;
}

// Returns the number of points in the line
std::size_t line_length(const vent_line& line)
{
    const point delta{line.b - line.a};
    return static_cast<std::size_t>(
        std::max(std::abs(delta.x), std::abs(delta.y)) + 1);
}

// Returns the delta between each point in the line
point line_step(const vent_line& line)
{
    point step{line.b - line.a};

    if (step.x != 0) {
        step.x /= std::abs(step.x);
    }
    if (step.y != 0) {
        step.y /= std::abs(step.y);
    }
    return step;
}

// Returns a range of all the points in the line
auto line_points(const vent_line& line)
{
    auto generator{[p = line.a, step = line_step(line)]() mutable {
        point prev{p};
        p += step;
        return prev;
    }};
    return rv::generate_n(generator, line_length(line));
}

// Returns a range of all the points in a range of all the lines
auto all_line_points(auto&& all_lines_range)
{
    return all_lines_range |
           rv::transform([](const auto& line) { return line_points(line); }) |
           rv::join;
}

auto solve_map(auto&& lines)
{
    std::map<point, int> point_counts;
    for (const auto p : all_line_points(lines)) {
        point_counts[p]++;
    }
    return r::count_if(point_counts | rv::values, [](int i) { return i > 1; });
}

aoc::solution_result day05map(std::string_view input)
{
    const std::vector<vent_line> all_vent_lines{parse_lines(input)};

    const auto count_a{
        solve_map(all_vent_lines | rv::filter(line_is_horiz_or_vert))};
    const auto count_b{solve_map(all_vent_lines)};
    return {count_a, count_b};
}

auto solve_sorted_points(auto&& lines)
{
    std::vector<point> points;
    points.reserve(500000);
    r::copy(all_line_points(lines), r::back_inserter(points));

    r::sort(points);

    auto repeats{r::distance(
        points | rv::sliding(2) |
        rv::filter([](const auto& p) { return p[0] == p[1]; }) |
        rv::transform([](const auto& p) { return p[0]; }) | rv::unique)};

    return repeats;
}

aoc::solution_result day05sort_vec(std::string_view input)
{
    const std::vector<vent_line> all_vent_lines{parse_lines(input)};

    const auto count_a{solve_sorted_points(all_vent_lines |
                                           rv::filter(line_is_horiz_or_vert))};
    const auto count_b{solve_sorted_points(all_vent_lines)};
    return {count_a, count_b};
}

}  // namespace aoc::year2021
