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
#include <cmath>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

using grid_t = dynamic_grid<char>;
using pos_t = vec2<int>;
using pos_t2 = vec2<std::int64_t>;

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

const auto equals_char{[](char c) { return [c](char c2) { return c == c2; }; }};
const auto empty_range{
    [](auto&& rng) -> bool { return r::all_of(rng, equals_char('.')); }};

std::vector<std::int64_t> new_rng_map(auto&& rng, std::int64_t expansion_factor)
{
    std::vector<std::int64_t> out;

    out.resize(rng.size());
    std::int64_t offset{0};
    for (int i{0}; i < static_cast<int>(out.size()); i++) {
        if (empty_range(rng[i])) {
            offset += (expansion_factor - 1);
        }
        out[i] = offset;
        offset++;
    }

    return out;
}

}  // namespace

aoc::solution_result day11(std::string_view input)
{
    const auto grid{parse_grid(input)};

    const auto manhattan_distance{[](pos_t2 a, pos_t2 b) {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }};
    const auto manhattan_distance_rng{[=](auto&& rng) {
        return manhattan_distance(std::get<0>(rng), std::get<1>(rng));
    }};

    const auto solve_for_expansion_factor{[&](std::int64_t expansion_factor) {
        std::vector<std::int64_t> new_col_map{
            new_rng_map(grid.cols(), expansion_factor)};
        std::vector<std::int64_t> new_row_map{
            new_rng_map(grid.rows(), expansion_factor)};

        std::vector<pos_t2> galaxy_positions;
        for (pos_t pos : grid.area().all_points()) {
            if (grid[pos] == '#') {
                galaxy_positions.push_back(
                    pos_t2{new_col_map[pos.x], new_row_map[pos.y]});
            }
        }

        return r::accumulate(
                   rv::cartesian_product(galaxy_positions, galaxy_positions) |
                       rv::transform(manhattan_distance_rng),
                   std::int64_t{0}) /
               2;
    }};

    std::int64_t part1{solve_for_expansion_factor(2)};
    std::int64_t part2{solve_for_expansion_factor(1000000)};

    return {part1, part2};
}

}  // namespace aoc::year2023
