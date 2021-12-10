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

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <set>
#include <stdexcept>
#include <vector>

namespace aoc::year2021 {

namespace {

using height_t = std::int8_t;
using point_t = vec2<int>;
using grid_t = dynamic_grid_adapter<std::vector<height_t>>;

std::array<vec2<int>, 4> get_neighbors(vec2<int> p)
{
    return {p + point_t{-1, 0}, p + point_t{1, 0}, p + point_t{0, -1},
            p + point_t{0, 1}};
}

std::set<point_t> get_low_points(const grid_t& grid)
{
    std::set<point_t> low_points;
    const rect<int> area{{0, 0}, {grid.width(), grid.height()}};
    for (auto p : area.all_points()) {
        auto h{grid[p]};
        const auto neighbors{get_neighbors(p)};
        auto adjacents{neighbors | rv::filter([&](vec2<int> n) {
                           return area.contains(n);
                       })};
        auto adjacent_heights{
            adjacents | rv::transform([&](point_t n) { return grid[n]; })};
        if (r::all_of(adjacent_heights, [=](height_t h2) { return h2 > h; })) {
            low_points.insert(p);
        }
    }
    return low_points;
}

std::set<point_t> discover_neighbors_in_basin(
    const grid_t& grid,
    const std::set<point_t>& discovered_basin,
    height_t max_level)
{
    std::set<point_t> new_neighbors;
    const rect<int> area{{0, 0}, {grid.width(), grid.height()}};
    for (auto p : discovered_basin) {
        auto point_neighbors{get_neighbors(p)};
        auto adjacents{point_neighbors | rv::filter([&](vec2<int> n) {
                           return area.contains(n);
                       })};
        for (auto n : adjacents) {
            if (grid[n] <= max_level && !discovered_basin.contains(n)) {
                new_neighbors.insert(n);
            }
        }
    }
    return new_neighbors;
}

point_t discover_basin_low_point(const grid_t& grid, point_t p)
{
    const height_t max_level{grid[p]};
    if (max_level == 9) {
        throw std::domain_error("height 9 points are not in a basin");
    }
    std::set<point_t> discovered_basin;
    discovered_basin.insert(p);

    std::set<point_t> new_neighbors{
        discover_neighbors_in_basin(grid, discovered_basin, max_level)};
    discovered_basin.insert(new_neighbors.begin(), new_neighbors.end());
    while (!new_neighbors.empty()) {
        new_neighbors =
            discover_neighbors_in_basin(grid, discovered_basin, max_level);
        discovered_basin.insert(new_neighbors.begin(), new_neighbors.end());
    }

    auto points_with_heights{rv::zip(
        discovered_basin, discovered_basin | rv::transform([&](point_t p2) {
                              return grid[p2];
                          }))};
    auto lowest_pair{r::min(points_with_heights, [](auto lhs, auto rhs) {
        return std::get<1>(lhs) < std::get<1>(rhs);
    })};
    return std::get<0>(lowest_pair);
}

}  // namespace

void print_grid(const grid_t& grid)
{
    const auto low_points{get_low_points(grid)};
    auto rows{grid.rows() | r::to<std::vector>};
    for (int y{0}; y < grid.height(); y++) {
        std::string line;
        for (int x{0}; x < grid.width(); x++) {
            std::string formatted;
            if (low_points.count({x, y})) {
                formatted = fmt::format("\x1B[33m{:1}\x1B[0m", grid[{x, y}]);
            }
            else {
                formatted = fmt::format("{:1}", grid[{x, y}]);
            }
            line += formatted;
        }
        fmt::print("{}\n", line);
    }
}

aoc::solution_result day09(std::string_view input)
{
    const auto lines{sv_lines(input)};
    const auto col_count{r::front(lines).size()};
    const auto row_count{static_cast<std::size_t>(r::distance(lines))};

    std::vector<height_t> data;
    data.resize(col_count * row_count);
    r::copy(lines | rv::join | rv::transform([](char c) {
                return static_cast<height_t>(c - '0');
            }),
            data.begin());
    // TODO: validate input better

    dynamic_grid_adapter grid{data, static_cast<int>(col_count)};

    int part_a_sum{0};
    for (auto p : get_low_points(grid)) {
        part_a_sum += grid[p] + 1;
    }

    const rect<int> area{{0, 0}, {grid.width(), grid.height()}};

    std::map<point_t, std::set<point_t>> all_basins;
    for (auto p : area.all_points()) {
        height_t h{grid[p]};
        if (h >= 9) {
            continue;
        }
        point_t low_point{discover_basin_low_point(grid, p)};
        all_basins[low_point].insert(p);
    }

    auto set_size{[](const std::set<point_t>& s) { return s.size(); }};

    auto sizes{all_basins | rv::values | rv::transform(set_size) |
               r::to<std::vector>};
    r::sort(sizes, std::greater<>{});

    auto part_b_product{sizes[0] * sizes[1] * sizes[2]};

    // print_grid(grid);

    return {part_a_sum, part_b_product};
}

}  // namespace aoc::year2021
