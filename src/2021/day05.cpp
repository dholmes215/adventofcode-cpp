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

#include <absl/container/btree_map.h>

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

rect<int> line_rect(const vent_line& line)
{
    std::array<point, 2> points{line.a, line.b};
    r::sort(points);
    point dimensions{points[1] - points[0] + point{1, 1}};

    // TODO: deal with bad rect input
    return rect<int>{points[0], dimensions};
}

bool line_is_horiz_or_vert(const vent_line& line)
{
    return line.a.x == line.b.x || line.a.y == line.b.y;
}

std::vector<point> line_points_a(const vent_line& line)
{
    return line_rect(line).all_points() | r::to<std::vector>;
}

std::vector<point> line_points_b(const vent_line& line)
{
    if (line_is_horiz_or_vert(line)) {
        return line_points_a(line);
    }

    // is it a 45 degree angle?
    point diff{line.b - line.a};
    diff.x = std::abs(diff.x);
    diff.y = std::abs(diff.y);
    if (diff.x != diff.y) {
        throw input_error{"not a 45 degree angle"};
    }

    std::vector<point> out;

    point step{line.b - line.a};
    step.x /= diff.x;
    step.y /= diff.y;

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
    absl::btree_map<point, int> point_counts;
    for (const auto& vl : lines) {
        for (const auto p : line_points_b(vl)) {
            point_counts[p]++;
        }
    }

    return r::count_if(point_counts | rv::values, [](int i) { return i > 1; });
};

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
