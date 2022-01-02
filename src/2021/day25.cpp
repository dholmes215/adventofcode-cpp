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

#include <string_view>

namespace aoc::year2021 {

namespace {

using grid_t = dynamic_grid<char>;
using vec_t = vec2<int>;

grid_t read_grid(std::string_view input)
{
    const auto lines{sv_lines(input) | r::to<std::vector>};
    grid_t grid{static_cast<int>(lines[0].size()),
                static_cast<int>(lines.size())};
    r::copy(lines | rv::join, grid.data().begin());
    return grid;
}

grid_t step(const grid_t& grid)
{
    grid_t next{grid};
    r::fill(next.data(), '.');
    for (const vec_t point : grid.area().all_points()) {
        vec_t east{point + vec_t{1, 0}};
        if (east.x >= grid.width()) {
            east.x = 0;
        }
        if (grid[point] == '>') {
            if (grid[east] == '.') {
                next[east] = '>';
            }
            else {
                next[point] = '>';
            }
        }
    }
    for (const vec_t point : grid.area().all_points()) {
        vec_t south{point + vec_t{0, 1}};
        if (south.y >= grid.height()) {
            south.y = 0;
        }
        if (grid[point] == 'v') {
            if (grid[south] != 'v' && next[south] != '>') {
                next[south] = 'v';
            }
            else {
                next[point] = 'v';
            }
        }
    }
    return next;
}

}  // namespace

aoc::solution_result day25(std::string_view input)
{
    grid_t grid{read_grid(input)};

    int i{1};
    auto next{step(grid)};

    bool changed = grid != next;
    while (changed) {
        i++;
        grid = next;
        next = step(grid);
        changed = grid != next;
    }

    return {i, "🎄"};
}

}  // namespace aoc::year2021
