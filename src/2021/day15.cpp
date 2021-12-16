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

// #include <term.hpp>

// #include <fmt/format.h>

#include <limits>
#include <queue>
#include <set>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

namespace {

using risk_level_t = int;
using risk_grid_t = dynamic_grid<risk_level_t>;
using point_t = vec2<int>;
using point_grid_t = dynamic_grid<point_t>;
constexpr risk_level_t infinity{std::numeric_limits<risk_level_t>::max()};
constexpr point_t undefined{-1, -1};

struct queue_entry {
    point_t vert;
    risk_level_t dist;
    friend auto operator<=>(const queue_entry& lhs,
                            const queue_entry& rhs) noexcept
    {
        return rhs.dist <=> lhs.dist;  // note rhs on left; we want min
    }
};

using queue_t = std::priority_queue<queue_entry>;

// void print_grid(const risk_grid_t& grid)
// {
//     for (const auto y : rv::iota(0, grid.height())) {
//         for (const auto x : rv::iota(0, grid.width())) {
//             point_t p{x, y};
//             fmt::print("{:1}", grid[p]);
//         }
//         fmt::print("\n");
//     }
// }

// void print_grid_highlighted(const risk_grid_t& grid,
//                             const std::vector<point_t>& path)
// {
//     dh::color color;
//     std::set<point_t> path_set(path.begin(), path.end());
//     for (const auto y : rv::iota(0, grid.height())) {
//         for (const auto x : rv::iota(0, grid.width())) {
//             point_t p{x, y};
//             if (path_set.contains(p)) {
//                 fmt::print("{0:yellow}{1:1}{0:reset}", color, grid[p]);
//             }
//             else {
//                 fmt::print("{0:blue}{1:1}{0:reset}", color, grid[p]);
//             }
//         }
//         fmt::print("{0:reset}\n", color);
//     }
// }

struct dijkstra_out {
    risk_grid_t dist;
    point_grid_t prev;
};

constexpr std::array<point_t, 4> directions{{{0, -1}, {1, 0}, {0, 1}, {-1, 0}}};

// TODO: extract this out
auto neighbors(rect<int> area, point_t p)
{
    return directions | rv::transform([p](const point_t d) { return p + d; }) |
           rv::filter([area](const point_t d) { return area.contains(d); });
}

dijkstra_out dijkstra(const risk_grid_t& grid, point_t start)
{
    dijkstra_out out{{grid.width(), grid.height()},
                     {grid.width(), grid.height()}};
    r::fill(out.dist.data(), infinity);
    r::fill(out.prev.data(), undefined);

    risk_grid_t& dist_grid{out.dist};
    point_grid_t& prev_grid{out.prev};
    dist_grid[start] = risk_level_t{0};

    queue_t q;
    for (point_t point : grid.area().all_points()) {
        q.push({point, dist_grid[point]});
    }

    while (!q.empty()) {
        queue_entry e{q.top()};
        q.pop();
        const auto u{e.vert};  // best vertex
        if (e.dist != dist_grid[u]) {
            // We re-added and already processed this vertex
            continue;
        }

        for (point_t neighbor_v : neighbors(grid.area(), u)) {
            const auto alt{dist_grid[u] + grid[neighbor_v]};
            if (alt < dist_grid[neighbor_v]) {
                dist_grid[neighbor_v] = alt;
                prev_grid[neighbor_v] = u;
                q.push({neighbor_v, alt});
            }
        }
    }

    return out;
}

// std::vector<point_t> make_path(const point_grid_t& dijkstra_prev,
//                                const point_t source,
//                                const point_t dest)
// {
//     std::vector<point_t> path;
//     point_t p{dest};
//     while (p != source) {
//         path.push_back(p);
//         p = dijkstra_prev[p];
//     }
//     return path;
// }

risk_grid_t expand_grid(const risk_grid_t& grid)
{
    const int grid_size{grid.width()};
    risk_grid_t expanded_grid{grid.width() * 5, grid.height() * 5};
    for (point_t new_point : expanded_grid.area().all_points()) {
        point_t old_point{new_point.x % grid_size, new_point.y % grid_size};
        risk_level_t x_repeat{new_point.x / grid_size};
        risk_level_t y_repeat{new_point.y / grid_size};
        risk_level_t increase{x_repeat + y_repeat};
        expanded_grid[new_point] = grid[old_point] + increase;
        if (expanded_grid[new_point] > 9) {
            expanded_grid[new_point] -= 9;
        }
    }
    return expanded_grid;
}

risk_level_t calculate_total_risk(const risk_grid_t& grid)
{
    const point_t source{0, 0};
    const point_t dest{grid.width() - 1, grid.height() - 1};

    auto [dist, prev]{dijkstra(grid, source)};

    return dist[dest];
}

}  // namespace

aoc::solution_result day15(std::string_view input)
{
    const auto lines{sv_lines(input)};
    const int grid_size{static_cast<int>(r::front(lines).size())};

    risk_grid_t grid{grid_size, grid_size};
    r::copy(lines | rv::join | rv::transform([](char c) { return c - '0'; }),
            grid.data().begin());

    const risk_level_t total_risk{calculate_total_risk(grid)};

    risk_grid_t expanded_grid{expand_grid(grid)};
    const risk_level_t total_risk_expanded{calculate_total_risk(expanded_grid)};

    return {total_risk, total_risk_expanded};
}

}  // namespace aoc::year2021
