//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>

#include <string_view>

namespace aoc::year2022 {

aoc::solution_result day08(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    const auto grid_width{static_cast<int>(r::front(lines).size())};
    const auto grid_height{static_cast<int>(r::distance(lines))};
    dynamic_grid<char> grid{grid_width, grid_height};
    r::copy(lines | rv::join, grid.data().begin());

    const auto visible{[&](auto p) {
        const auto tree_height{grid[p]};
        const auto row{grid.row(p.y)};
        const auto col{grid.col(p.x)};
        const auto shorter{[&](auto t) { return t < tree_height; }};
        const auto west_approach{row | rv::take(p.x)};
        const auto east_approach{row | rv::reverse |
                                 rv::take(grid_width - p.x - 1)};
        const auto north_approach{col | rv::take(p.y)};
        const auto south_approach{col | rv::reverse |
                                  rv::take(grid_height - p.y - 1)};
        return r::all_of(west_approach, shorter) ||
               r::all_of(east_approach, shorter) ||
               r::all_of(north_approach, shorter) ||
               r::all_of(south_approach, shorter);
    }};
    const auto visible_count{r::count_if(grid.area().all_points(), visible)};

    const auto scenic_score{[&](auto p) {
        const auto tree_height{grid[p]};
        const auto row{grid.row(p.y)};
        const auto col{grid.col(p.x)};
        const auto shorter{[&](auto t) { return t < tree_height; }};

        const auto west{row | rv::take(p.x) | rv::reverse};
        const auto east{row | rv::drop(p.x + 1)};
        const auto north{col | rv::take(p.y) | rv::reverse};
        const auto south{col | rv::drop(p.y + 1)};

        const auto west_shorter{west | rv::take_while(shorter)};
        const auto east_shorter{east | rv::take_while(shorter)};
        const auto north_shorter{north | rv::take_while(shorter)};
        const auto south_shorter{south | rv::take_while(shorter)};

        const auto west_view{west | rv::take(r::distance(west_shorter) + 1)};
        const auto east_view{east | rv::take(r::distance(east_shorter) + 1)};
        const auto north_view{north | rv::take(r::distance(north_shorter) + 1)};
        const auto south_view{south | rv::take(r::distance(south_shorter) + 1)};

        return r::distance(west_view) * r::distance(east_view) *
               r::distance(south_view) * r::distance(north_view);
    }};
    const auto highest_scenic_score{
        r::max(grid.area().all_points() | rv::transform(scenic_score))};

    return {visible_count, highest_scenic_score};
}

}  // namespace aoc::year2022
