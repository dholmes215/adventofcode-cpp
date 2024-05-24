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

constexpr std::size_t rock_count1{2022};
constexpr std::size_t rock_count2{1000000000000ULL};
constexpr int room_height{1000000};

using grid_t = dynamic_grid<char>;
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
    grid_t out{9, room_height};
    r::fill(out.data(), '.');
    r::fill(out.row(out.height() - 1), '-');
    r::fill(out.col(0), '|');
    r::fill(out.col(out.width() - 1), '|');
    out[{0, out.height() - 1}] = '+';
    out[{out.width() - 1, out.height() - 1}] = '+';
    return out;
}

// void print_grid(const grid_t& grid, int height)
// {
//     auto subgrid{
//         grid.subgrid({{0, grid.height() - height}, {grid.width(), height}})};
//     for (const auto& row : subgrid.rows()) {
//         fmt::print("{}\n", row | r::to<std::string>);
//     }
//     fmt::print("\n");
// }

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

std::vector<std::int64_t> drop_blocks(grid_t& grid,
                                      std::size_t block_count,
                                      const std::vector<pos_t>& jet_patterns)
{
    const auto jet_cycle(jet_patterns | rv::cycle);
    auto jet_iter{jet_cycle.begin()};

    const auto piece_cycle(pieces | rv::cycle);
    auto piece_iter{piece_cycle.begin()};

    auto highest_rock_row{grid.height() - 1};
    std::vector<std::int64_t> tower_height_by_block;
    tower_height_by_block.reserve(static_cast<std::size_t>(grid.height()));

    for (std::size_t r{0}; r < block_count; r++) {
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
        highest_rock_row =
            std::min(highest_rock_row, pos.y + (4 - piece_height(piece)));
        tower_height_by_block.push_back(grid.height() - highest_rock_row - 1);
    }

    return tower_height_by_block;
}

}  // namespace

aoc::solution_result day17(std::string_view input)
{
    const auto jet_patterns(trim(input) | rv::transform(jet_move) |
                            r::to<std::vector>);

    auto grid1{initialize_room()};
    // print_grid(grid);
    std::vector<std::int64_t> tower_height1{
        drop_blocks(grid1, rock_count1, jet_patterns)};
    // print_grid(grid, 3000);

    auto grid2{initialize_room()};
    const std::size_t rocks_to_actually_drop2{200000};
    std::vector<std::int64_t> tower_height2{
        drop_blocks(grid2, rocks_to_actually_drop2, jet_patterns)};

    // detect cycle period
    std::vector<std::int64_t> differences;
    differences.resize(tower_height2.size());
    r::adjacent_difference(tower_height2, differences.begin());
    r::reverse(differences);
    const auto section{differences | rv::take(20) | r::to<std::vector>};
    auto search_result{r::search(differences | rv::drop(1), section)};

    const auto cycle_period{static_cast<std::size_t>(
        r::distance(differences.begin(), search_result.begin()))};

    const auto tower_height_reversed{tower_height2 | rv::reverse |
                                     r::to<std::vector>};
    const std::size_t cycle_height_diff{static_cast<std::size_t>(
        tower_height_reversed[0] - tower_height_reversed[cycle_period])};
    const std::size_t rocks_after_drop{rock_count2 - rocks_to_actually_drop2};
    const std::size_t cycles_to_multiply{rocks_after_drop / cycle_period + 1};
    const auto rocks_calculated{cycles_to_multiply * cycle_period};
    const auto final_tower_height2{
        cycles_to_multiply * cycle_height_diff +
        static_cast<std::size_t>(
            tower_height_reversed[rocks_calculated - rocks_after_drop])};

    return {tower_height1.back(), final_tower_height2};
}

}  // namespace aoc::year2022
