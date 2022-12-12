//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_graph.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <string_view>
#include <vector>

namespace aoc::year2022 {

namespace {

using point_t = vec2<int>;
struct parse_result_t {
    dynamic_grid<char> grid;
    point_t start;
    point_t end;
};

parse_result_t parse_input(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    const int width{static_cast<int>(lines[0].size())};
    const int height{static_cast<int>(lines.size())};

    parse_result_t out{{width, height}, {}, {}};
    r::copy(lines | rv::join, out.grid.data().data());
    for (auto point : out.grid.area().all_points()) {
        if (out.grid[point] == 'S') {
            out.start = point;
            out.grid[point] = 'a';
        }
        if (out.grid[point] == 'E') {
            out.end = point;
            out.grid[point] = 'z';
        }
    }
    return out;
}

constexpr std::array<point_t, 4> directions{{{0, -1}, {1, 0}, {0, 1}, {-1, 0}}};

}  // namespace

aoc::solution_result day12(std::string_view input)
{
    auto [grid_, start, end]{parse_input(input)};
    const auto& grid{grid_};
    const auto adj_func1{[&](point_t p) {
        const auto can_move{[p, &grid](point_t dest) {
            return grid.area().contains(dest) && (grid[dest] - grid[p] <= 1);
        }};

        return directions | rv::transform([p](point_t d) { return d + p; }) |
               rv::filter(can_move);
    }};
    auto path1{bfs_path(adj_func1, start, end)};

    const auto adj_func2{[&](point_t p) {
        const auto can_move{[p, &grid](point_t dest) {
            return grid.area().contains(dest) && (grid[p] - grid[dest] <= 1);
        }};

        return directions | rv::transform([p](point_t d) { return d + p; }) |
               rv::filter(can_move);
    }};
    auto path2{
        bfs_accept(adj_func2, end, [&](point_t p) { return grid[p] == 'a'; })};

    return {path1.size() - 1, path2.size() - 1};
}

}  // namespace aoc::year2022
