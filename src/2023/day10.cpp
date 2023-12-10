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

#include <fmt/ranges.h>

#include <algorithm>
#include <deque>
#include <map>
#include <set>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

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

constexpr const std::array<pos_t, 4> cardinal_directions{
    {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}};

const std::string pipe_chars{"|-LJ7F"};

std::vector<pos_t> neighbor_directions(char c)
{
    switch (c) {
        case '|':
            return {{0, -1}, {0, 1}};
        case '-':
            return {{-1, 0}, {1, 0}};
        case 'L':
            return {{0, -1}, {1, 0}};
        case 'J':
            return {{0, -1}, {-1, 0}};
        case '7':
            return {{0, 1}, {-1, 0}};
        case 'F':
            return {{0, 1}, {1, 0}};
    }
    return {};
}

std::vector<pos_t> potential_neighbors(pos_t pos, char c)
{
    std::vector<pos_t> neighbor_dirs{neighbor_directions(c)};
    return neighbor_dirs | rv::transform([=](pos_t n) { return pos + n; }) |
           r::to<std::vector>;
}

bool is_connected(const grid_t& grid, pos_t a, pos_t b)
{
    return r::contains(potential_neighbors(a, grid[a]), b) &&
           r::contains(potential_neighbors(b, grid[b]), a);
}

std::vector<pos_t> actual_neighbors(const grid_t& grid, pos_t pos)
{
    std::vector<pos_t> out;
    for (pos_t n : potential_neighbors(pos, grid[pos])) {
        if (is_connected(grid, pos, n)) {
            out.push_back(n);
        }
    }
    return out;
}

void fill_start_pipe_char(grid_t& grid, pos_t start)
{
    for (char c : pipe_chars) {
        grid[start] = c;
        if (actual_neighbors(grid, start).size() == 2) {
            return;
        }
    }
    throw input_error("Failed to detect start pipe character");
}

// std::string pos_to_string(pos_t pos) {
//     return fmt::format("({},{})", pos.x, pos.y);
// }

// void print_grid(const grid_t& grid)
// {
//     for (int row{0}; row < grid.height(); row++) {
//         for (int col{0}; col < grid.width(); col++) {
//             fmt::print("{}", grid[{col, row}]);
//         }
//         fmt::print("\n");
//     }
// }

void flood_fill(const grid_t& grid, pos_t pos, char c)
{
    std::set<pos_t> to_fill{pos};
    while (!to_fill.empty()) {
        pos_t filling{r::front(to_fill)};
        auto neighbors{cardinal_directions |
                       rv::transform([&](pos_t p2) { return p2 + filling; }) |
                       rv::filter([&](pos_t p2) {
                           return grid.area().contains(p2) && grid[p2] == '.';
                       })};
        for (pos_t neighbor : neighbors) {
            to_fill.insert(neighbor);
        }
        grid[filling] = c;
        to_fill.erase(filling);
    }
}

std::vector<pos_t> outside_directions(pos_t prev_pos, pos_t current_pos, char c)
{
    pos_t move{current_pos - prev_pos};
    switch (c) {
        case '|':
            if (move == pos_t{0, 1}) {  // Down
                return {{1, 0}};   // Right
            }
            return {{-1, 0}};  // Left
        case '-':
            if (move == pos_t{1, 0}) {  // Right
                return {{0, -1}};  // Up
            }
            return {{0, 1}};  // Down
        case 'L':
            if (move == pos_t{0, 1}) {  // Down
                return {};         // Empty
            }
            return {{-1, 0}, {0, 1}};  // Left and Down
        case 'J':
            if (move == pos_t{0, 1}) {          // Down
                return {{1, 0}, {0, -1}};  // Right and Down
            }
            return {};  // Empty
        case '7':
            if (move == pos_t{1, 0}) {          // Right
                return {{0, -1}, {1, 0}};  // Up and Right
            }
            return {};  // Empty
        case 'F':
            if (move == pos_t{0, -1}) {          // Up
                return {{-1, 0}, {0, -1}};  // Left and Up
            }
            return {};  // Empty
    }
    return {};
}

}  // namespace

aoc::solution_result day10(std::string_view input)
{
    grid_t grid{parse_grid(input)};
    pos_t start{};
    for (auto pos : grid.area().all_points()) {
        if (grid[pos] == 'S') {
            start = pos;
        }
    }

    fill_start_pipe_char(grid, start);

    std::map<pos_t, int> visited_distances{{start, 0}};
    std::deque<pos_t> to_visit{actual_neighbors(grid, start) |
                               r::to<std::deque>};
    while (!to_visit.empty()) {
        pos_t visiting_pos{to_visit.front()};
        to_visit.pop_front();
        if (!visited_distances.contains(visiting_pos)) {
            for (pos_t neighbor : actual_neighbors(grid, visiting_pos)) {
                if (visited_distances.contains(neighbor)) {
                    if (!visited_distances.contains(visiting_pos)) {
                        visited_distances[visiting_pos] =
                            visited_distances[neighbor] + 1;
                    }
                }
                else {
                    to_visit.push_back(neighbor);
                }
            }
        }
    }

    int part1{r::max(visited_distances | rv::values)};

    // Edit the map to replace junk pipes with '.'
    for (auto pos : grid.area().all_points()) {
        if (!visited_distances.contains(pos)) {
            grid[pos] = '.';
        }
    }

    if (grid[{0, 0}] != '.') {
        throw input_error(
            "grid[0,0] isn't '.' and I'm too lazy to accomodate that");
    }

    // Flood fill the outside with 'O'
    flood_fill(grid, {0, 0}, 'O');

    // Detect a pipe touching the outside as a new starting point
    pos_t start2;
    for (pos_t p : grid.area().all_points()) {
        if (r::contains(pipe_chars, grid[p])) {
            start2 = p;
            break;
        }
    }
    if (grid[start2] != 'F') {
        throw input_error("Topmost pipe is not 'F'; not what I expected");
    }

    // Follow the pipe clockwise, flood filling any ground on its left with 'O'
    pos_t current{start2};
    pos_t prev{start2 + pos_t{0, 1}};
    do {
        for (pos_t outside_dir : outside_directions(prev, current, grid[current])) {
            if (grid[outside_dir + current] == '.') {
                flood_fill(grid, outside_dir + current, 'O');
            }
        }
        pos_t next;
        for (pos_t neighbor : actual_neighbors(grid, current)) {
            if (neighbor != prev) {
                next = neighbor;
                break;
            }
        }
        prev = current;
        current = next;
    } while (current != start2); 

    // Count the '.'s
    auto part2{r::count(grid.data(), '.')};

    return {part1, part2};
}

}  // namespace aoc::year2023
