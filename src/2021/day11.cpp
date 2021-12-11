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

#include <cstdint>
#include <set>
#include <string_view>

namespace aoc::year2021 {

namespace {

constexpr int grid_size{10};
using energy_t = int8_t;
using grid_t = static_grid<energy_t, grid_size, grid_size>;
using point_t = vec2<int>;  // TODO: member of grid
using flashcount_t = int;

std::int8_t digit_to_number(char c)
{
    return c - '0';
}

// dh::color color;
// void print_grid(const grid_t& grid, int step)
// {
//     fmt::print("Step {}:\n", step);
//     for (auto y : rv::iota(0, 10)) {
//         for (auto x : rv::iota(0, 10)) {
//             energy_t e = grid[{x, y}];
//             if (e == 0) {
//                 fmt::print("{0:yellow}{1}{0:reset}", color, grid[{x, y}]);
//             }
//             else {
//                 fmt::print("{}", grid[{x, y}]);
//             }
//         }
//         fmt::print("\n");
//     }
// }

}  // namespace

void increment_grid(grid_t& grid)
{
    for (auto& oct : grid.data()) {
        oct++;
    }
}

constexpr std::array<point_t, 9> directions{
    {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}}};

auto get_neighbors(point_t p)
{
    return directions | rv::transform([p](auto p2) { return p + p2; }) |
           rv::filter([](point_t p2) { return grid_t::area.contains(p2); });
}

flashcount_t run_some_flashes(grid_t& grid)
{
    flashcount_t flashcount{0};
    std::set<point_t> flashed;
    for (point_t p : grid_t::area.all_points()) {
        if (grid[p] > 9) {
            grid[p] = 0;
            flashcount++;
            flashed.insert(p);
        }
    }
    for (point_t p : grid_t::area.all_points()) {
        auto neighbors = get_neighbors(p);
        if (flashed.contains(p)) {
            for (point_t p2 : neighbors) {
                if (grid[p2] > 0) {
                    grid[p2]++;
                }
            }
        }
    }
    return flashcount;
}

flashcount_t run_all_flashes(grid_t& grid)
{
    flashcount_t totalcount{0};
    while (auto count = run_some_flashes(grid)) {
        totalcount += count;
    }
    return totalcount;
}

flashcount_t run_step(grid_t& grid)
{
    increment_grid(grid);
    return run_all_flashes(grid);
}

aoc::solution_result day11(std::string_view input)
{
    grid_t grid{};
    r::copy(input | rv::filter(is_digit) | rv::transform(digit_to_number),
            grid.data().begin());

    flashcount_t count_a{0};
    int i{0};
    for (i = 0; i < 100; i++) {
        count_a += run_step(grid);
    }

    while (!r::all_of(grid.data(), [](energy_t e) { return e == 0; })) {
        run_step(grid);
        i++;
    }

    return {count_a, i};
}

}  // namespace aoc::year2021
