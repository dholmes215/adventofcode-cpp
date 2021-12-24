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

#include <cassert>
#include <chrono>
#include <cstdint>
#include <map>
#include <queue>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <vector>

namespace aoc::year2021 {

namespace {

constexpr std::string_view end_state_sv_a{R"(
#############
#...........#
###A#B#C#D###
  #A#B#C#D#
  #########
)"};

constexpr std::string_view inserted_line_1{"  #D#C#B#A#  "};
constexpr std::string_view inserted_line_2{"  #D#B#A#C#  "};

using grid_a_t = static_grid<char, 13, 5>;
using grid_b_t = static_grid<char, 13, 7>;
using vec_t = vec2<int>;
using cost_t = std::uint64_t;

grid_a_t parse_input_a(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    grid_a_t grid;
    r::fill(grid.data(), ' ');
    for (int row{0}; row < 5; row++) {
        r::copy(lines[static_cast<std::size_t>(row)], grid.row(row).begin());
    }
    return grid;
}

grid_b_t parse_input_b(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    grid_b_t grid;
    r::fill(grid.data(), ' ');
    for (int row{0}; row < 3; row++) {
        r::copy(lines[static_cast<std::size_t>(row)], grid.row(row).begin());
    }
    r::copy(inserted_line_1, grid.row(3).begin());
    r::copy(inserted_line_2, grid.row(4).begin());
    for (int row{5}; row < 7; row++) {
        r::copy(lines[static_cast<std::size_t>(row - 2)],
                grid.row(row).begin());
    }

    return grid;
}

grid_b_t end_grid_b()
{
    grid_b_t out{parse_input_b(end_state_sv_a)};
    r::copy(out.row(5), out.row(3).begin());
    r::copy(out.row(5), out.row(4).begin());
    return out;
}

// std::string color_string(char c)
// {
//     dh::color color;
//     switch (c) {
//         case '#':
//             return fmt::format("#{0:reset}", color);
//         case '.':
//             return ".";
//         case ' ':
//             return " ";
//         case 'A':
//             return fmt::format("{0:red}{0:bold}A{0:reset}", color);
//         case 'B':
//             return fmt::format("{0:yellow}{0:bold}B{0:reset}", color);
//         case 'C':
//             return fmt::format("{0:green}{0:bold}C{0:reset}", color);
//         case 'D':
//             return fmt::format("{0:blue}{0:bold}D{0:reset}", color);
//         default:  // unexpected
//             return fmt::format("{0:magenta}{0:bold}{1}{0:reset}", color, c);
//     }
// }

// void print_burrow(const auto& grid)
// {
//     for (int row{0}; row < grid.height(); row++) {
//         for (int col{0}; col < grid.width(); col++) {
//             fmt::print("{}", color_string(grid[{col, row}]));
//         }
//         fmt::print("\n");
//     }
// }

// void print_pair(const auto& grid1, const auto& grid2)
// {
//     dynamic_grid<char> grid{30, grid1.height()};
//     r::fill(grid.data(), ' ');
//     grid[{14, grid.height() / 2}] = '-';
//     grid[{15, grid.height() / 2}] = '>';
//     for (int row{0}; row < grid1.height(); row++) {
//         for (int col{0}; col < grid1.width(); col++) {
//             grid[{col, row}] = grid1[{col, row}];
//             grid[{col + 17, row}] = grid2[{col, row}];
//         }
//     }
//     for (int row{0}; row < grid.height(); row++) {
//         for (int col{0}; col < grid.width(); col++) {
//             fmt::print("{}", color_string(grid[{col, row}]));
//         }
//         fmt::print("\n");
//     }
// }

bool is_amphipod(char c)
{
    return c == 'A' || c == 'B' || c == 'C' || c == 'D';
}

using amphipod_positions_a_t = std::array<vec_t, 8>;
using amphipod_positions_b_t = std::array<vec_t, 16>;

template <typename Grid>
auto locate_amphipods(const Grid& grid) noexcept
{
    if constexpr (std::is_same_v<Grid, grid_a_t>) {
        amphipod_positions_a_t out;
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
    else {
        amphipod_positions_b_t out;
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
}

constexpr bool is_hallway(vec_t p) noexcept
{
    return p.y == 1 && p.x >= 1 && p.x <= 11;
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

// Return nullopt of room cannot be entered, or the destination coordinates in
// the room if it can be
template <typename Grid>
std::optional<vec_t> can_enter_room(const Grid& grid, char c) noexcept
{
    const auto col{end_room_col(c)};
    const rect<int> room_rect{{col, 2}, {1, grid.height() - 3}};
    const auto room_grid{grid.subgrid(room_rect)};
    auto data{room_grid.data()};
    auto first_not_empty{r::find_if(data, [](char c2) { return c2 != '.'; })};
    const bool all_c{r::all_of(first_not_empty, data.end(),
                               [c](char c2) { return c2 == c; })};
    if (!all_c) {
        return std::nullopt;
    }

    const auto empty_spaces{
        r::distance(room_grid.data().begin(), first_not_empty)};
    if (empty_spaces == 0) {
        return std::nullopt;
    }
    return {vec_t{col, static_cast<int>(empty_spaces + 1)}};
}

constexpr std::array<vec_t, 4> directions{{
    {-1, 0},
    {1, 0},
    {0, -1},
    {0, 1},
}};

template <typename Grid>
struct move {
    Grid dest;
    cost_t cost;
};

template <typename Grid>
std::optional<move<Grid>> move_into_room(const Grid& grid, vec_t moving)
{
    const auto target{can_enter_room(grid, grid[moving])};
    if (!target) {
        return std::nullopt;
    }

    const vec_t outside_room{target->x, 1};
    const vec_t adjacent{moving.x + ((target->x > moving.x) ? 1 : -1), 1};
    const auto hallway_rect{rect_from_corners(outside_room, adjacent)};
    const auto hallway_grid{grid.subgrid(hallway_rect)};
    if (!r::all_of(hallway_grid.data(), [](char c) { return c == '.'; })) {
        return std::nullopt;
    }

    const auto steps{
        static_cast<cost_t>(hallway_rect.dimensions.x + target->y - 1)};
    move<Grid> out{grid, steps * move_cost(grid[moving])};
    std::swap(out.dest[moving], out.dest[*target]);
    return out;
}

template <typename Grid>
std::vector<move<Grid>> enumerate_possible_moves(const Grid& grid)
{
    std::vector<move<Grid>> out;

    (void)directions;

    const auto positions{locate_amphipods(grid)};
    // XXX We would partition the positions to put hallway amphipods first,
    // except locate_amphipods happens to return them that way.  TODO: Do that
    // anyways
    for (const vec_t moving : positions) {
        // If in hallway, we can only move into a room
        if (is_hallway(moving)) {
            // Can the target room be moved into?
            const auto maybe_move{move_into_room(grid, moving)};
            if (maybe_move) {
                out.push_back(*maybe_move);
                // If any amphipod can move into a room, then there will be no
                // path with a better cost than doing so, so don't bother
                // considering other moves and just return prematurely.
                return out;
            }
        }

        // If in a room, we can only move into certain hallway positions, and
        // can only move if we're on top.
        const vec_t outside_room{moving.x, 1};
        const bool on_top{grid[moving + vec_t{0, -1}] == '.'};
        if (on_top) {
            constexpr std::array<int, 7> target_cols{1, 2, 4, 6, 8, 10, 11};
            for (const int col : target_cols) {
                const vec_t target{col, 1};
                const auto hallway_rect{
                    rect_from_corners(outside_room, target)};
                const auto hallway_grid{grid.subgrid(hallway_rect)};

                if (r::all_of(hallway_grid.data(),
                              [](char c) { return c == '.'; })) {
                    // We have a clear path to this target.
                    const auto steps{static_cast<cost_t>(
                        hallway_rect.dimensions.x + moving.y - 2)};
                    move<Grid> new_move{grid, steps * move_cost(grid[moving])};
                    std::swap(new_move.dest[moving], new_move.dest[target]);
                    out.push_back(new_move);
                }
            }
        }
    }

    return out;
}

template <typename Grid>
struct dijkstra_out {
    std::map<Grid, cost_t> dist;
    std::map<Grid, Grid> prev;
};

template <typename Grid>
struct queue_entry {
    Grid vert;
    cost_t dist;
    friend auto operator<=>(const queue_entry& lhs,
                            const queue_entry& rhs) noexcept
    {
        return rhs.dist <=> lhs.dist;  // note rhs on left; we want min
    }
};

template <typename Grid>
dijkstra_out<Grid> dijkstra(Grid start)
{
    dijkstra_out<Grid> out;
    out.dist[start] = 0;

    using queue_t = std::priority_queue<queue_entry<Grid>>;
    queue_t q;
    q.push({start, 0});

    while (!q.empty()) {
        queue_entry<Grid> e{q.top()};
        q.pop();
        const auto u{e.vert};  // best vertex

        const auto neighbors{enumerate_possible_moves(u)};
        for (auto neighbor : neighbors) {
            const auto& v{neighbor.dest};
            const auto alt{e.dist + neighbor.cost};
            if (!out.dist.contains(v) || alt < out.dist.at(v)) {
                out.dist[v] = alt;
                out.prev[v] = u;
                q.push({v, alt});
            }
        }
    }

    return out;
}

// template <typename Grid>
// std::optional<std::vector<Grid>> reconstruct_path(
//     const Grid& start,
//     const Grid& end,
//     const std::map<Grid, Grid>& prev)
// {
//     std::vector<Grid> out;
//     Grid cur{end};
//     while (cur != start) {
//         out.push_back(cur);
//         if (!prev.contains(cur)) {
//             return std::nullopt;
//         }
//         cur = prev.at(cur);
//     }
//     out.push_back(start);
//     std::reverse(out.begin(), out.end());
//     return out;
// }

}  // namespace

aoc::solution_result day23(std::string_view input)
{
    const grid_a_t start{parse_input_a(input)};
    const grid_a_t end{parse_input_a(end_state_sv_a)};

    // print_pair(start, end);

    auto search_result{dijkstra(start)};
    const auto part_a_energy{search_result.dist.contains(end)
                                 ? search_result.dist.at(end)
                                 : cost_t{0}};

    const grid_b_t start_b{parse_input_b(input)};
    const grid_b_t end_b{end_grid_b()};

    // print_pair(start_b, end_b);

    auto search_result_b{dijkstra(start_b)};
    const cost_t part_b_energy{search_result_b.dist.at(end_b)};

    // {
    //     auto path{reconstruct_path(start_b, end_b, search_result_b.prev)};
    //     if (!path) {
    //         fmt::print("No path found!\n");
    //     }
    //     else {
    //         fmt::print("\n\n\n\n\n\n\n\n");
    //         for (const auto& state : *path) {
    //             fmt::print("\033[8;A\n");
    //             print_burrow(state);
    //             using namespace std::chrono_literals;
    //             std::this_thread::sleep_for(250ms);
    //         }
    //     }
    // }

    return {part_a_energy, part_b_energy};
}

}  // namespace aoc::year2021
