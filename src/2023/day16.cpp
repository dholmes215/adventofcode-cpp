//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <coro_generator.hpp>

#include <fmt/ranges.h>

#include <algorithm>
#include <set>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

using int_t = std::int64_t;
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

constexpr const pos_t left{-1, 0};
constexpr const pos_t right{1, 0};
constexpr const pos_t up{0, -1};
constexpr const pos_t down{0, 1};

struct beam_head {
    pos_t pos{0, 0};
    pos_t dir{right};
    friend auto operator<=>(const beam_head&,
                            const beam_head&) noexcept = default;
};

void move_all_heads(const grid_t& grid,
                    grid_t& energized_grid,
                    std::vector<beam_head>& heads,
                    std::set<beam_head>& heads_already_computed)
{
    std::vector<beam_head> new_heads;
    const auto move_head_straight{[&](beam_head& head) {
        head.pos += head.dir;

        if (grid.area().contains(head.pos)) {
            energized_grid[head.pos] = '#';
            new_heads.push_back(head);
        }
    }};

    for (const beam_head& head : heads) {
        if (heads_already_computed.contains(head)) {
            continue;
        }
        heads_already_computed.insert(head);
        bool move_straight{grid[head.pos] == '.' ||
                           (grid[head.pos] == '|' && head.dir.x == 0) ||
                           (grid[head.pos] == '-' && head.dir.y == 0)};
        if (move_straight) {
            beam_head new_head{head};
            move_head_straight(new_head);
        }
        else if (grid[head.pos] == '|') {
            beam_head new_head_up{head.pos, up};
            beam_head new_head_down{head.pos, down};
            move_head_straight(new_head_up);
            move_head_straight(new_head_down);
        }
        else if (grid[head.pos] == '-') {
            beam_head new_head_left{head.pos, left};
            beam_head new_head_right{head.pos, right};
            move_head_straight(new_head_left);
            move_head_straight(new_head_right);
        }
        else if (grid[head.pos] == '/') {
            beam_head new_head{head};
            if (new_head.dir == left) {
                new_head.dir = down;
            }
            else if (new_head.dir == right) {
                new_head.dir = up;
            }
            else if (new_head.dir == down) {
                new_head.dir = left;
            }
            else {  // up
                new_head.dir = right;
            }
            move_head_straight(new_head);
        }
        else {  // '\'
            beam_head new_head{head};
            if (new_head.dir == left) {
                new_head.dir = up;
            }
            else if (new_head.dir == right) {
                new_head.dir = down;
            }
            else if (new_head.dir == down) {
                new_head.dir = right;
            }
            else {  // up
                new_head.dir = left;
            }
            move_head_straight(new_head);
        }
    }
    heads = new_heads;
}

auto test_start_tile(const grid_t& grid, beam_head start)
{
    std::vector<beam_head> heads{start};

    grid_t energized_grid(grid.width(), grid.height());
    r::fill(energized_grid.data(), '.');
    energized_grid[start.pos] = '#';

    std::set<beam_head> heads_already_computed;

    while (!heads.empty()) {
        move_all_heads(grid, energized_grid, heads, heads_already_computed);
    }

    return r::count(energized_grid.data(), '#');
}

Generator<beam_head> enumerate_all_start_tiles(const grid_t& grid)
{
    // top row
    for (int x{0}; x < grid.width(); x++) {
        co_yield beam_head{{x, 0}, down};
    }
    // bottom row
    for (int x{0}; x < grid.width(); x++) {
        co_yield beam_head{{x, grid.height() - 1}, up};
    }
    // left column
    for (int y{0}; y < grid.height(); y++) {
        co_yield beam_head{{0, y}, right};
    }
    // right column
    for (int y{0}; y < grid.height(); y++) {
        co_yield beam_head{{grid.width() - 1, y}, left};
    }
}

}  // namespace

aoc::solution_result day16(std::string_view input)
{
    const grid_t grid(parse_grid(input));

    const auto part1{test_start_tile(grid, {{0, 0}, right})};

    int_t part2{0};
    for (const beam_head& head : enumerate_all_start_tiles(grid)) {
        part2 = std::max(part2, test_start_tile(grid, head));
    }

    return {part1, part2};
}

}  // namespace aoc::year2023
