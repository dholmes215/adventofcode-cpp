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

#include <limits>
#include <queue>
#include <set>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

namespace {

}  // namespace

using risk_level_t = int;
using risk_grid_t = dynamic_grid_adapter<std::vector<risk_level_t>>;
using point_t = vec2<int>;
using point_grid_t = dynamic_grid_adapter<std::vector<point_t>>;
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

void print_grid(const risk_grid_t& grid)
{
    for (const auto y : rv::iota(0, grid.height())) {
        for (const auto x : rv::iota(0, grid.width())) {
            point_t p{x, y};
            fmt::print("{:1}", grid[p]);
        }
        fmt::print("\n");
    }
}

struct dijkstra_out {
    std::vector<risk_level_t> dist;
    std::vector<point_t> prev;
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
    dijkstra_out out;
    out.dist.resize(static_cast<std::size_t>(grid.width() * grid.height()),
                    infinity);
    out.prev.resize(static_cast<std::size_t>(grid.width() * grid.height()),
                    undefined);

    risk_grid_t dist_grid(out.dist, grid.width());
    point_grid_t prev_grid(out.prev, grid.width());
    dist_grid[start] = risk_level_t{0};

    queue_t q;
    for (point_t p : grid.area().all_points()) {
        q.push({p, dist_grid[p]});
    }
    while (!q.empty()) {
        queue_entry e{q.top()};
        q.pop();
        if (e.dist == infinity) {
            continue;
        }

        const auto u{e.vert};
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

std::vector<point_t> make_path(const point_grid_t& dijkstra_prev,
                               const point_t source,
                               const point_t dest)
{
    std::vector<point_t> path;
    point_t p{dest};
    while (p != source) {
        path.push_back(p);
        p = dijkstra_prev[p];
    }
    return path;
}

aoc::solution_result day15(std::string_view input)
{
    const auto lines{sv_lines(input)};
    const int grid_size{static_cast<int>(r::front(lines).size())};

    std::vector<risk_level_t> data;
    data.resize(static_cast<std::size_t>(grid_size * grid_size));
    r::copy(lines | rv::join | rv::transform([](char c) { return c - '0'; }),
            data.begin());
    risk_grid_t grid{data, grid_size};

    const point_t source{0, 0};
    const point_t dest{grid.width() - 1, grid.height() - 1};

    auto [dist, prev]{dijkstra(grid, source)};

    // FIXME: adapter doesn't work for `const` vector
    risk_grid_t dist_grid{dist, grid.width()};
    point_grid_t prev_grid{prev, grid.width()};

    const auto path{make_path(prev_grid, source, dest)};
    risk_level_t total_risk{0};
    for (const auto p : path) {
        total_risk += grid[p];
    }

    // Part 2

    std::vector<risk_level_t> data_b;
    data_b.resize(data.size() * 25);
    risk_grid_t grid_b{data_b, grid_size * 5};
    for (point_t p : grid_b.area().all_points()) {
        point_t p_a{p.x % grid_size, p.y % grid_size};
        risk_level_t x_repeat{p.x / grid_size};
        risk_level_t y_repeat{p.y / grid_size};
        risk_level_t increase{x_repeat + y_repeat};
        grid_b[p] = grid[p_a] + increase;
        if (grid_b[p] > 9) {
            grid_b[p] -= 9;
        }
    }
    // print_grid(grid_b);

    // TODO: implement operator* for vec2
    const point_t dest_b{grid.width() * 5 - 1, grid.height() * 5 - 1};

    auto [dist_b, prev_b]{dijkstra(grid_b, source)};
    risk_grid_t dist_grid_b{dist_b, grid_b.width()};
    point_grid_t prev_grid_b{prev_b, grid_b.width()};

    const auto path_b{make_path(prev_grid_b, source, dest_b)};
    risk_level_t total_risk_b{0};
    for (const auto p : path_b) {
        total_risk_b += grid_b[p];
    }
    return {total_risk, total_risk_b};
}

}  // namespace aoc::year2021
