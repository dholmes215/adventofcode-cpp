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

// #include <ctre.hpp>

#include <regex>

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
    // vent_line out;
    // auto vent_matcher{ctre::match<"(\\d+),(\\d+) -> (\\d+),(\\d+)">};
    // using namespace ctre::literals;
    // auto [whole, x1, y1, x2, y2] = vent_matcher(sv);
    // if ( whole) {
    //     return vent_line{
    //         vec2<int>{to_int(x1.to_view()), to_int(x2.to_view())},
    //         vec2<int>{to_int(y1.to_view()), to_int(y1.to_view())}};
    // }
    // throw input_error{fmt::format("failed to parse line: {}", line)};

    const std::regex vent_regex{"(\\d+),(\\d+) -> (\\d+),(\\d+)"};
    std::smatch m;

    const std::string str{line};
    if (!std::regex_match(str, m, vent_regex)) {
        throw input_error{fmt::format("failed to parse line: {}", line)};
    }
    auto x1{std::stoi(m[1].str())};
    auto y1{std::stoi(m[2].str())};
    auto x2{std::stoi(m[3].str())};
    auto y2{std::stoi(m[4].str())};

    return vent_line{{x1, y1}, {x2, y2}};
}

rect<int> line_rect(const vent_line& line)
{
    std::array<point, 2> points{line.a, line.b};
    r::sort(points);
    point dimensions{points[1] - points[0] + point{1, 1}};
    // fmt::print("{},{} {},{} {},{}\n", points[0].x, points[0].y, points[1].x,
    //            points[1].y, dimensions.x, dimensions.y);
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

void print_grid(heap_grid<int, 1000, 1000>& grid)
{
    for (int y{0}; y < 10; y++) {
        for (int x{0}; x < 10; x++) {
            int i{grid[{x, y}]};
            char c{(i == 0) ? '.' : static_cast<char>('0' + i)};
            fmt::print("{}", c);
        }
        fmt::print("\n");
    }
}

aoc::solution_result day05(std::string_view input)
{
    const std::vector<vent_line> vent_lines{parse_lines(input)};
    const auto horiz_vert_vent_lines{
        vent_lines | rv::filter(line_is_horiz_or_vert) | r::to<std::vector>};

    const int count_a{[&]() {
        heap_grid<int, 1000, 1000> grid{};

        std::map<point, int> point_counts;
        for (const auto& vl : horiz_vert_vent_lines) {
            for (const auto p : line_points_a(vl)) {
                grid[p]++;
                point_counts[p]++;
            }
        }

        auto values{point_counts | rv::values};
        auto values_above_1{values | rv::filter([](int i) { return i > 1; })};

        const int count{static_cast<int>(r::distance(values_above_1))};

        // const int count{0};

        // for (const auto vl : horiz_vert_vent_lines) {
        //     fmt::print("{},{} -> {},{}\n", vl.a.x, vl.a.y, vl.b.x,
        //     vl.b.y);
        // }
        return count;
    }()};

    const int count_b{[&]() {
        heap_grid<int, 1000, 1000> grid{};

        std::map<point, int> point_counts;
        for (const auto& vl : vent_lines) {
            for (const auto p : line_points_b(vl)) {
                grid[p]++;
                point_counts[p]++;
            }
        }

        print_grid(grid);

        auto values{point_counts | rv::values};
        auto values_above_1{values | rv::filter([](int i) { return i > 1; })};

        const int count{static_cast<int>(r::distance(values_above_1))};

        // const int count{0};

        // for (const auto vl : horiz_vert_vent_lines) {
        //     fmt::print("{},{} -> {},{}\n", vl.a.x, vl.a.y, vl.b.x,
        //     vl.b.y);
        // }
        return count;
    }()};

    return {count_a, count_b};
}

}  // namespace aoc::year2021
