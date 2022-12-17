//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <fmt/ranges.h>

#include <string_view>

namespace aoc::year2022 {

namespace {

constexpr int rock_count{2022};
constexpr int tallest_rock{4};
constexpr int tower_height{rock_count * tallest_rock + 10};

using grid_t = heap_grid<char, 9, tower_height>;
using piece_t = static_grid<char, 4, 4>;
using pos_t = vec2<int>;

// clang-format off
constexpr char piece_a[]{
"...."
"...."
"...."
"####"
};

constexpr char piece_b[]{
"...."
".#.."
"###."
".#.."
};

constexpr char piece_c[]{
"...."
"..#."
"..#."
"###."
};

constexpr char piece_d[]{
"#..."
"#..."
"#..."
"#..."
};

constexpr char piece_e[]{
"...."
"...."
"##.."
"##.."
};

// clang-format on

piece_t init_piece(std::string_view chars)
{
    piece_t out;
    r::copy(chars.substr(0, 16), out.data().begin());
    return out;
}

std::array pieces{init_piece(piece_a), init_piece(piece_b), init_piece(piece_c),
                  init_piece(piece_d), init_piece(piece_e)};

int piece_height(const piece_t& piece)
{
    int i{0};
    for (const auto& row : piece.rows() | rv::reverse) {
        if (r::find(row, '#') == r::end(row)) {
            return i;
        }
        i++;
    }
    return i;
}

grid_t initialize_room()
{
    grid_t out;
    r::fill(out.data(), '.');
    r::fill(out.row(out.height() - 1), '-');
    r::fill(out.col(0), '|');
    r::fill(out.col(out.width() - 1), '|');
    out[{0, out.height() - 1}] = '+';
    out[{out.width() - 1, out.height() - 1}] = '+';
    return out;
}

void print_grid(const grid_t& grid)
{
    auto subgrid{grid.subgrid({{0, grid.height() - 25}, {grid.width(), 25}})};
    for (const auto& row : subgrid.rows()) {
        fmt::print("{}\n", row | r::to<std::string>);
    }
    fmt::print("\n");
}

// void print_piece_in_room(const grid_t& grid, const piece_t& piece, pos_t pos)
// {
//     rect<int> r{{0, grid.height() - 10}, {grid.width(), 10}};
//     auto subgrid{grid.subgrid(r)};
//     dynamic_grid<char> print_grid{subgrid.width(), subgrid.height()};
//     r::copy(subgrid.data(), print_grid.data().begin());
//     auto placement_subgrid{
//         print_grid.subgrid({{pos - r.base}, piece.area.dimensions})};
//     for (pos_t p : piece.area.all_points()) {
//         if (piece[p] == '#') {
//             placement_subgrid[p] = '@';
//         }
//     }
//     for (const auto& row : print_grid.rows()) {
//         fmt::print("{}\n", row | r::to<std::string>);
//     }
//     fmt::print("\n");
// }

bool check_collision(const grid_t& grid, const piece_t& piece, pos_t pos)
{
    auto subgrid{grid.subgrid({pos, piece.area.dimensions})};
    for (pos_t p : piece.area.all_points()) {
        if (piece[p] == '#' && subgrid[p] != '.') {
            return true;
        }
    }
    return false;
}

void place_piece(grid_t& grid, const piece_t& piece, pos_t pos)
{
    auto subgrid{grid.subgrid({pos, piece.area.dimensions})};
    for (pos_t p : piece.area.all_points()) {
        if (piece[p] == '#') {
            subgrid[p] = '#';
        }
    }
}

pos_t jet_move(char c)
{
    switch (c) {
        case '<':
            return {-1, 0};
        case '>':
            return {1, 0};
    }
    throw input_error(fmt::format("Invalid character: '{}'", c));
}

}  // namespace

aoc::solution_result day17(std::string_view input)
{
    const auto jet_patterns(trim(input) | rv::transform(jet_move) |
                            r::to<std::vector>);
    const auto jet_cycle(jet_patterns | rv::cycle);
    const auto piece_cycle(pieces | rv::cycle);
    auto jet_iter{jet_cycle.begin()};
    auto piece_iter{piece_cycle.begin()};

    auto grid{initialize_room()};

    print_grid(grid);

    auto highest_rock_row{grid.height() - 1};

    for (int r{0}; r < rock_count; r++) {
        auto piece{*piece_iter++};
        pos_t pos{3, highest_rock_row - piece.height() - 3};
        // print_piece_in_room(grid, piece, pos);
        bool placed{false};
        while (!placed) {
            auto move{*jet_iter++};
            if (!check_collision(grid, piece, pos + move)) {
                pos += move;
            }
            // print_piece_in_room(grid, piece, pos);
            pos_t down{0, 1};
            if (check_collision(grid, piece, pos + down)) {
                place_piece(grid, piece, pos);
                placed = true;
                // print_grid(grid);
            }
            else {
                pos += down;
                // print_piece_in_room(grid, piece, pos);
            }
        }
        highest_rock_row = pos.y + (4 - piece_height(piece));
    }
    print_grid(grid);

    return {grid.height() - highest_rock_row, ""};
}

}  // namespace aoc::year2022
