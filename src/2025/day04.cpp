//
// Copyright (c) 2025 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>

#include <string_view>
#include <vector>

namespace aoc::year2025 {

namespace {

dynamic_grid<char> parse_grid(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    dynamic_grid<char> grid{static_cast<int>(lines[0].size()),
                            static_cast<int>(lines.size())};
    const auto tiles{lines | rv::join |
                     rv::transform([](char c) { return c; })};
    r::copy(tiles, grid.data().data());
    return grid;
}

constexpr const std::array<vec2<int>, 8> neighbor_directions{
    {{0, -1}, {0, 1}, {-1, 0}, {1, 0}, {1, -1}, {1, 1}, {-1, 1}, {-1, -1}}};

auto neighbor_tiles(const dynamic_grid<char>& grid, vec2<int> p)
{
    return neighbor_directions | rv::transform([p](auto d) { return p + d; }) |
           rv::filter([&grid](auto p) { return grid.area().contains(p); }) |
           rv::transform([&grid](auto p) { return grid[p]; });
}

int count_neighbor_rolls(const dynamic_grid<char>& grid, vec2<int> p)
{
    return r::count(neighbor_tiles(grid, p), '@');
}

int check_position(const dynamic_grid<char>& grid, vec2<int> p)
{
    return grid[p] == '@' && count_neighbor_rolls(grid, p) < 4;
}

void remove_all_possible_rolls(dynamic_grid<char>& grid)
{
    bool removed = true;
    while (removed) {
        removed = false;
        for (const auto p : grid.area().all_points()) {
            if (check_position(grid, p)) {
                grid[p] = 'x';
                removed = true;
            }
        }
    }
}

}  // namespace

aoc::solution_result day04(std::string_view input)
{
    input = trim(input);
    auto grid = parse_grid(input);

    const auto a = r::count_if(grid.area().all_points(), [&](auto p) { return check_position(grid, p); });

    const auto rolls_before = r::count(grid.data(), '@');
    remove_all_possible_rolls(grid);
    const auto rolls_after = r::count(grid.data(), '@');

    const auto b = rolls_before - rolls_after;

    return {a, b};
}

}  // namespace aoc::year2025
