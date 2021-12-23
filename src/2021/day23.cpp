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

#include <term.hpp>

#include <fmt/format.h>

#include <cassert>
#include <cstdint>
#include <map>
#include <queue>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

namespace {

}  // namespace

constexpr std::string_view end_state_sv{R"(
#############
#...........#
###A#B#C#D###
  #A#B#C#D#
  #########
)"};

std::string color_string(char c)
{
    dh::color color;
    switch (c) {
        case '#':
            return fmt::format("#{0:reset}", color);
        case '.':
            return ".";
        case ' ':
            return " ";
        case 'A':
            return fmt::format("{0:red}{0:bold}A{0:reset}", color);
        case 'B':
            return fmt::format("{0:yellow}{0:bold}B{0:reset}", color);
        case 'C':
            return fmt::format("{0:green}{0:bold}C{0:reset}", color);
        case 'D':
            return fmt::format("{0:blue}{0:bold}D{0:reset}", color);
        default:  // unexpected
            return fmt::format("{0:magenta}{0:bold}{1}{0:reset}", color, c);
    }
}

using grid_t = static_grid<char, 13, 5>;
using vec_t = vec2<int>;
using cost_t = std::uint64_t;

struct burrow_state {
    grid_t grid;

    // Once an amphipod starts moving out of a room, it can continue to move
    // freely only until another amphipod moves.
    vec_t moving_out{0, 0};

    // Once an amphipod is moving is attempting to move into a room, no other
    // amphipod can move until it succeeds.
    vec_t moving_in{0, 0};

    friend auto operator<=>(const burrow_state& lhs,
                            const burrow_state& rhs) noexcept = default;
};

burrow_state parse_input(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    burrow_state out;
    grid_t& grid{out.grid};
    r::fill(grid.data(), ' ');
    for (int row{0}; row < 5; row++) {
        r::copy(lines[static_cast<std::size_t>(row)], grid.row(row).begin());
    }
    return out;
}

void print_burrow(const burrow_state& burrow)
{
    const grid_t& grid{burrow.grid};
    for (int row{0}; row < 5; row++) {
        for (int col{0}; col < 13; col++) {
            fmt::print("{}", color_string(grid[{col, row}]));
        }
        fmt::print("\n");
    }
}

void print_pair(const burrow_state& burrow1, const burrow_state& burrow2)
{
    static_grid<char, 30, 5> grid;
    r::fill(grid.data(), ' ');
    grid[{14, 2}] = '-';
    grid[{15, 2}] = '>';
    for (int row{0}; row < 5; row++) {
        for (int col{0}; col < 13; col++) {
            grid[{col, row}] = burrow1.grid[{col, row}];
            grid[{col + 17, row}] = burrow2.grid[{col, row}];
        }
    }
    for (int row{0}; row < 5; row++) {
        for (int col{0}; col < 30; col++) {
            fmt::print("{}", color_string(grid[{col, row}]));
        }
        fmt::print("\n");
    }
}

bool is_amphipod(char c)
{
    return c == 'A' || c == 'B' || c == 'C' || c == 'D';
}

using amphipod_positions_t = std::array<vec_t, 8>;

amphipod_positions_t locate_amphipods(const grid_t& grid) noexcept
{
    amphipod_positions_t out;
    auto iter{out.begin()};
    for (const vec_t point : grid.area.all_points()) {
        const auto c{grid[point]};
        if (is_amphipod(c)) {
            *iter++ = point;
        }
    }
    assert(iter == out.end());
    return out;
}

constexpr bool is_hallway(vec_t p) noexcept
{
    return p.y == 1 && p.x >= 1 && p.x <= 11;
}

constexpr bool is_immediately_outside_room(vec_t p) noexcept
{
    return p.y == 1 && (p.x == 3 || p.x == 5 || p.x == 7 || p.x == 9);
}

int end_room_col(char c)
{
    switch (c) {
        case 'A':
            return 3;
        case 'B':
            return 5;
        case 'C':
            return 7;
        case 'D':
            return 9;
    }
    throw std::domain_error(fmt::format("'{}' is not an amphipod", c));
}

cost_t move_cost(char c)
{
    switch (c) {
        case 'A':
            return 1;
        case 'B':
            return 10;
        case 'C':
            return 100;
        case 'D':
            return 1000;
    }
    throw std::domain_error(fmt::format("'{}' is not an amphipod", c));
}

// Precondition: p is outside a room
bool can_enter_room(const grid_t& grid, vec_t p) noexcept
{
    assert(is_immediately_outside_room(p));
    if (grid[p + vec_t{0, 1}] != '.') {
        return false;
    }
    if (p.x != end_room_col(grid[p])) {
        return false;
    }
    if (grid[p + vec_t{0, 2}] == '.') {
        return true;
    }
    if (p.x != end_room_col(grid[p + vec_t{0, 2}])) {
        return false;
    }
    return true;
}

constexpr std::array<vec_t, 4> directions{{
    {-1, 0},
    {1, 0},
    {0, -1},
    {0, 1},
}};

struct move {
    burrow_state dest;
    cost_t cost;
};

std::vector<move> enumerate_possible_moves(const burrow_state& burrow)
{
    const grid_t& grid{burrow.grid};
    std::vector<move> out;
    if (burrow.moving_out != vec_t{0, 0}) {
        vec_t moving{burrow.moving_out};
        // We must do something with this amphipod:
        // 1: move anywhere outside the room
        for (const vec_t dir : directions) {
            const vec_t dest{moving + dir};
            if (grid[dest] == '.') {
                if (is_hallway(dest)) {
                    move m{burrow, move_cost(grid[moving])};
                    m.dest.grid[dest] = grid[moving];
                    m.dest.grid[moving] = '.';
                    m.dest.moving_out = dest;
                    out.push_back(m);
                }
            }
        }

        // 2: if not immediately outside a room, can mark as not moving
        if (!is_immediately_outside_room(moving)) {
            move m{burrow, cost_t{0}};
            m.dest.moving_out = {0, 0};
            out.push_back(m);
        }
    }
    else if (burrow.moving_in != vec_t{0, 0}) {
        vec_t moving{burrow.moving_in};
        // We must do something with this amphipod:
        // 1: move anywhere except into a disallowed room:
        for (const vec_t dir : directions) {
            const vec_t dest{moving + dir};
            if (grid[dest] == '.') {
                if (is_hallway(dest) || (is_immediately_outside_room(moving) &&
                                         can_enter_room(grid, moving))) {
                    move m{burrow, move_cost(grid[moving])};
                    m.dest.grid[dest] = grid[moving];
                    m.dest.grid[moving] = '.';
                    m.dest.moving_in = dest;
                    out.push_back(m);
                }
            }
        }
        // 2: If inside a room, can mark as not moving
        if (!is_hallway(moving)) {
            move m{burrow, cost_t{0}};
            m.dest.moving_in = {0, 0};
            out.push_back(m);
        }
    }
    else {
        const amphipod_positions_t positions{locate_amphipods(grid)};
        for (const vec_t moving : positions) {
            // We can:
            if (!is_hallway(moving)) {
                // 1: move anyone within a room
                for (const vec_t dir : directions) {
                    const vec_t dest{moving + dir};
                    if (grid[dest] == '.' && !is_hallway(dest)) {
                        move m{burrow, move_cost(grid[moving])};
                        m.dest.grid[dest] = grid[moving];
                        m.dest.grid[moving] = '.';
                        out.push_back(m);
                    }
                }
                // 2: select an amphipod to move out of a room
                move m{burrow, cost_t{0}};
                m.dest.moving_out = moving;
                out.push_back(m);
            }
            else {
                // 3: select an amphipod to move into a room
                move m{burrow, cost_t{0}};
                m.dest.moving_in = moving;
                out.push_back(m);
            }
        }
    }

    return out;
}

using dist_map_t = std::map<burrow_state, cost_t>;
using prev_map_t = std::map<burrow_state, burrow_state>;

struct dijkstra_out {
    dist_map_t dist;
    prev_map_t prev;
};

struct queue_entry {
    burrow_state vert;
    cost_t dist;
    friend auto operator<=>(const queue_entry& lhs,
                            const queue_entry& rhs) noexcept
    {
        return rhs.dist <=> lhs.dist;  // note rhs on left; we want min
    }
};

using queue_t = std::priority_queue<queue_entry>;

dijkstra_out dijkstra(burrow_state start)
{
    dijkstra_out out;
    out.dist[start] = 0;

    queue_t q;
    q.push({start, 0});

    while (!q.empty()) {
        queue_entry e{q.top()};
        q.pop();
        const auto u{e.vert};  // best vertex

        for (auto neighbor : enumerate_possible_moves(u)) {
            const auto& v{neighbor.dest};
            const auto alt{e.dist + neighbor.cost};
            if (!out.dist.contains(v) || alt < out.dist.at(v)) {
                out.dist[v] = alt;
                out.prev[v] = u;
                q.push({v, alt});
            }
        }
        // fmt::print("Queue size: {}\n", q.size());
    }

    return out;
}

aoc::solution_result day23(std::string_view input)
{
    const burrow_state start{parse_input(input)};
    const burrow_state end{parse_input(end_state_sv)};

    print_pair(start, end);

    auto search_result{dijkstra(start)};
    // for (const auto& [state, prev] : search_result.prev) {
    //     print_pair(state, prev);
    //     fmt::print("---------\n");
    // }
    fmt::print("{} nodes\n", search_result.dist.size());
    fmt::print("{} nodes\n", search_result.prev.size());

    return {search_result.dist.at(end), search_result.dist.at(start)};
}

}  // namespace aoc::year2021
