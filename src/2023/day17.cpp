//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_graph.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>

#include <algorithm>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

using int_t = std::int64_t;
using heat_loss_t = std::int8_t;
using grid_t = dynamic_grid<heat_loss_t>;
using pos_t = vec2<int>;

grid_t parse_grid(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    grid_t grid{static_cast<int>(lines[0].size()),
                static_cast<int>(lines.size())};
    const auto tiles{lines | rv::join |
                     rv::transform([](char c) { return c - '0'; })};
    r::copy(tiles, grid.data().data());
    return grid;
}

struct crucible_state {
    pos_t pos;
    pos_t direction;
    int direction_moves;  // How many blocks in this direction we've moved;
                          // cannot exceed 3
    friend auto operator<=>(const crucible_state& lhs,
                            const crucible_state& rhs) noexcept = default;
};

struct crucible_queue_entry {
    crucible_state vert;
    int_t dist{0};
    friend auto operator<=>(const crucible_queue_entry& lhs,
                            const crucible_queue_entry& rhs) noexcept
    {
        return lhs.dist <=> rhs.dist;
    }
};

constexpr const pos_t north{0, -1};
constexpr const pos_t south{0, 1};
constexpr const pos_t west{-1, 0};
constexpr const pos_t east{1, 0};
constexpr const std::array<pos_t, 4> directions{north, south, west, east};

auto neighbors(const crucible_state& state,
               const grid_t& grid,
               int min_forward,
               int max_forward)
{
    if (state.direction_moves > 0 && state.direction_moves < min_forward) {
        crucible_queue_entry out;
        auto& [vert, dist]{out};
        vert = state;
        vert.pos += vert.direction;
        if (!grid.area().contains(vert.pos)) {
            // Can't move at all; return empty vector
            return std::vector<crucible_queue_entry>{};
        }
        dist += grid[vert.pos];
        vert.direction_moves++;
        return std::vector<crucible_queue_entry>{out};
    }

    std::array<crucible_queue_entry, 4> out{};
    for (int i{0}; i < 4; i++) {
        auto& [vert, dist]{out[i]};
        vert = state;
        vert.pos += directions[i];
        // dist += grid[vert.pos];
        vert.direction = directions[i];
        if (vert.direction == state.direction) {
            vert.direction_moves++;
        }
        else {
            vert.direction_moves = 1;
        }
    }

    const auto filter{[&](const crucible_queue_entry& e) {
        return e.vert.direction_moves <= max_forward &&
               grid.area().contains(e.vert.pos) &&
               (e.vert.direction != (state.direction * -1));
    }};
    auto out_vec{out | rv::filter(filter) | r::to<std::vector>};
    for (crucible_queue_entry& e : out_vec) {
        auto& [vert, dist]{e};
        dist += grid[vert.pos];
    }
    return out_vec;
}

struct path_graph {
    using vertex_type = crucible_state;
    using queue_entry = crucible_queue_entry;
    using cost_type = int_t;

    const vertex_type root() const { return start_; }
    bool accept(const vertex_type& v) const noexcept { return v.pos == end_; }
    auto adjacencies(const vertex_type& v) const
    {
        return neighbors(v, grid_, min_forward_, max_forward_);
    }

    const grid_t& grid_;
    const vertex_type& start_;
    const pos_t end_;
    const int min_forward_;
    const int max_forward_;
};

}  // namespace

// void print_result(const dijkstra_out<path_graph>& result,
//                   const grid_t& grid,
//                   pos_t start)
// {
//     crucible_state end{*result.end};

//     std::vector<crucible_state> path;
//     path.push_back(end);
//     while (path.back().pos != start) {
//         path.push_back(result.prev.at(path.back()));
//     }
//     for (const auto& v : path) {
//         fmt::print("{}\n", v.pos);
//     }

//     for (int row{0}; row < grid.height(); row++) {
//         for (int col{0}; col < grid.width(); col++) {
//             auto path_found{r::find_if(path, [&](const auto& v) {
//                 return v.pos == pos_t{col, row};
//             })};
//             if (path_found != r::end(path)) {
//                 fmt::print("#");
//             }
//             else {
//                 fmt::print("{}", grid[{col, row}]);
//             }
//         }
//         fmt::print("\n");
//     }
// }

aoc::solution_result day17(std::string_view input)
{
    grid_t grid{parse_grid(input)};

    pos_t end{grid.width() - 1, grid.height() - 1};
    crucible_state start_state{{0, 0}, east, 0};

    path_graph part1_graph{grid, start_state, end, 0, 3};
    auto part1_result{dijkstra(part1_graph)};
    crucible_state part1_end{*part1_result.end};
    auto part1{part1_result.dist.at(part1_end)};

    path_graph part2_graph{grid, start_state, end, 4, 10};
    auto part2_result{dijkstra(part2_graph)};
    crucible_state part2_end{*part2_result.end};
    auto part2{part2_result.dist.at(part2_end)};

    // print_result(part1_result, grid, start_state.pos);
    return {part1, part2};
}

}  // namespace aoc::year2023
