//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <algorithm>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace aoc::year2023 {

namespace {

using int_t = std::int64_t;
using grid_t = dynamic_grid<char>;
using pos_t = vec2<int>;

grid_t parse_grid(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    grid_t grid{static_cast<int>(lines[0].size()),
                static_cast<int>(lines.size())};
    const auto tiles{lines | rv::join |
                     rv::transform([](char c) { return c; })};
    r::copy(tiles, grid.data().data());
    return grid;
}

void tilt_north(grid_t& grid)
{
    for (auto&& col : grid.cols()) {
        for (auto&& section : col | rv::split('#')) {
            std::string copy{section | r::to<std::string>};
            r::sort(copy);
            r::copy(copy | rv::reverse, section.begin());
        }
    }
}

void tilt_south(grid_t& grid)
{
    for (auto&& col : grid.cols()) {
        for (auto&& section : col | rv::split('#')) {
            std::string copy{section | r::to<std::string>};
            r::sort(copy);
            r::copy(copy, section.begin());
        }
    }
}

void tilt_west(grid_t& grid)
{
    for (auto&& row : grid.rows()) {
        for (auto&& section : row | rv::split('#')) {
            std::string copy{section | r::to<std::string>};
            r::sort(copy);
            r::copy(copy | rv::reverse, section.begin());
        }
    }
}

void tilt_east(grid_t& grid)
{
    for (auto&& row : grid.rows()) {
        for (auto&& section : row | rv::split('#')) {
            std::string copy{section | r::to<std::string>};
            r::sort(copy);
            r::copy(copy, section.begin());
        }
    }
}

void spin_once(grid_t& grid)
{
    tilt_north(grid);
    tilt_west(grid);
    tilt_south(grid);
    tilt_east(grid);
}

int_t calculate_load(const grid_t& grid)
{
    const auto is_rounded_rock{[&grid](pos_t p) { return grid[p] == 'O'; }};
    const auto pos_load{[&grid](pos_t p) { return grid.height() - p.y; }};
    return r::accumulate(grid.area().all_points() |
                             rv::filter(is_rounded_rock) |
                             rv::transform(pos_load),
                         int_t{0});
}

auto cycle_generator(const grid_t& grid)
{
    auto func{[copy = grid]() mutable -> const grid_t& {
        spin_once(copy);
        return copy;
    }};

    return rv::generate(func);
}

std::size_t hash_char_range(auto&& rng) {
    std::size_t hash{0};
    for (char c : rng) {
        hash = hash ^ (c << 1);
    }
    return hash;
}

struct hash_grid
{
    std::size_t operator()(const grid_t& grid) const noexcept
    {
        return hash_char_range(grid.data());
    }
};

}  // namespace

aoc::solution_result day14(std::string_view input)
{
    grid_t grid(parse_grid(input));

    grid_t part1_grid{grid};
    tilt_north(part1_grid);
    int_t part1{calculate_load(part1_grid)};

    grid_t part2_grid{grid};
    int_t spin_count{0};

    std::unordered_map<grid_t, int_t, hash_grid> when_encountered;
    std::vector<int_t> loads;
    
    while (!when_encountered.contains(part2_grid)) {
        when_encountered[part2_grid] = spin_count;
        loads.push_back(calculate_load(part2_grid));
        spin_once(part2_grid);
        spin_count++;
    }

    int_t cycle_period{spin_count - when_encountered.at(part2_grid)};
    int_t cycle_count{(1000000000-spin_count) / cycle_period + 1};
    int_t part2_solution_spin_count{1000000000 - (cycle_count * cycle_period)};
    int_t part2{loads[part2_solution_spin_count]};
    
    return {part1, part2};
}

}  // namespace aoc::year2023
