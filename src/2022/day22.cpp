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

#include <fmt/format.h>

#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace aoc::year2022 {

namespace {

}  // namespace

using turn_t = char;    // 'L' or 'R'
using facing_t = char;  // '<' or '>' or '^' or 'v'
using dist_t = int;
using grid_t = dynamic_grid<char>;
using coord_t = vec2<int>;
using move_t = std::variant<dist_t, turn_t>;

struct pos_t {
    coord_t coords;
    facing_t facing;
};

const int edge_len_example{4};
const int edge_len_real{50};

using side_t = vec2<int>;
enum class corner_t { ne, nw, se, sw };

struct coord_range_t {
    coord_t first;
    coord_t last;
};

std::vector<move_t> parse_moves(std::string_view line)
{
    return line | rv::chunk_by([](char c1, char c2) {
               return is_digit(c1) == is_digit(c2);
           }) |
           rv::transform([](auto chars) -> move_t {
               auto as_sv{sv(chars)};
               if (as_sv == "L") {
                   return 'L';
               }
               if (as_sv == "R") {
                   return 'R';
               }
               return to_int(as_sv);
           }) |
           r::to<std::vector>;
}

facing_t turn(facing_t f, turn_t t)
{
    if (t != 'L' && t != 'R') {
        throw input_error(fmt::format("Unsupported facing character '{}'", f));
    }
    switch (f) {
        case '<':
            return t == 'L' ? 'v' : '^';
        case '>':
            return t == 'L' ? '^' : 'v';
        case '^':
            return t == 'L' ? '<' : '>';
        case 'v':
            return t == 'L' ? '>' : '<';
    }
    throw input_error(fmt::format("Unsupported facing character '{}'", f));
}

coord_t facing_step(facing_t f)
{
    switch (f) {
        case '<':
            return {-1, 0};
        case '>':
            return {1, 0};
        case '^':
            return {0, -1};
        case 'v':
            return {0, 1};
    }
    throw input_error(fmt::format("Unsupported facing character '{}'", f));
}

pos_t move1(const grid_t& board, pos_t p, const move_t& m)
{
    pos_t out{p};
    if (const turn_t* maybe_turn = std::get_if<turn_t>(&m)) {
        out.facing = turn(out.facing, *maybe_turn);
    }
    else {
        dist_t dist{std::get<dist_t>(m)};
        auto step{facing_step(out.facing)};
        for (int i{0}; i < dist; i++) {
            auto next{out.coords + step};
            next = {(next.x + board.width()) % board.width(),
                    (next.y + board.height()) % board.height()};
            while (board[next] == ' ') {
                next += step;
                next = {(next.x + board.width()) % board.width(),
                        (next.y + board.height()) % board.height()};
            }
            if (board[next] == '#') {
                break;
            }
            out.coords = next;
        }
    }
    return out;
}

int score_facing(facing_t f)
{
    switch (f) {
        case '>':
            return 0;
        case 'v':
            return 1;
        case '<':
            return 2;
        case '^':
            return 3;
    }
    throw input_error(fmt::format("Unsupported facing character '{}'", f));
}

int score_pos(const pos_t& pos)
{
    return 1000 * (pos.coords.y + 1) + 4 * (pos.coords.x + 1) +
           score_facing(pos.facing);
}

coord_t side_corner_pos(side_t side, corner_t corner, int edge_len)
{
    coord_t out{side * edge_len};
    if (corner == corner_t::ne || corner == corner_t::se) {
        out.x += edge_len - 1;
    }
    if (corner == corner_t::sw || corner == corner_t::se) {
        out.y += edge_len - 1;
    }
    return out;
}

auto coord_mapping(coord_range_t c_in, coord_range_t c_out)
{
    coord_t in_delta{c_in.first - c_in.last};
    coord_t out_delta{c_out.first - c_out.last};

    enum class axis_t { x, y };
    axis_t in_axis{in_delta.x == 0 ? axis_t::y : axis_t::x};

    coord_t out_step{out_delta.x / std::abs(out_delta.x),
                     out_delta.y / std::abs(out_delta.y)};

    return [=](const coord_t& c) {
        coord_t delta{c_in.first - c};

        int distance{in_axis == axis_t::x ? delta.x : delta.y};
        distance = std::abs(distance);

        return c_out.first + (out_step * distance);
    };
}

struct edge_t {
    side_t side;
    corner_t corner1;
    corner_t corner2;
};

using edge_pair_t = std::pair<edge_t, edge_t>;

const std::array edge_pairs_example{
    edge_pair_t{{{0, 1}, corner_t::ne, corner_t::nw},
                {{2, 0}, corner_t::nw, corner_t::ne}},
    edge_pair_t{{{1, 1}, corner_t::ne, corner_t::nw},
                {{2, 0}, corner_t::sw, corner_t::nw}},
    edge_pair_t{{{2, 0}, corner_t::ne, corner_t::se},
                {{3, 2}, corner_t::se, corner_t::ne}}};
// 0,1 ne-nw to 2,0 nw-ne
// 1,1 ne-nw to 2,0 sw-nw
// 2,0 ne-se to 3,2 se-ne
// 1,2 ne-se to 3,2 ne-nw
// 3,2 se-sw to 0,1 nw-sw
// 2,2 sw-se to 0,1 se-sw
// 2,2 sw-nw to 1,1 sw-se

// // FIXME: Need to come up with a general solution to these that's not just
// // my own hardcoded input
// pos_t edge_transform_example(pos_t p)
// {
//     auto [x, y]{p.coords};
//     switch (p.facing) {
//         case '^': {

//             if (y == 3 && x >= 4 && x <= 7) {
//                 int new_x{7};
//                 int new_y
//             }
//             return 0;
//         }
//     }
//     throw input_error(
//         fmt::format("Unsupported facing character '{}'", p.facing));
// }

// pos_t edge_transform_real(pos_t p)
// {
//     //
// }

// pos_t move2(const grid_t& board, pos_t p, const move_t& m)
// {
//     pos_t out{p};
//     if (const turn_t* maybe_turn = std::get_if<turn_t>(&m)) {
//         out.facing = turn(out.facing, *maybe_turn);
//     }
//     else {
//         dist_t dist{std::get<dist_t>(m)};
//         auto step{facing_step(out.facing)};
//         for (int i{0}; i < dist; i++) {
//             auto next{out.coords + step};
//             next = {(next.x + board.width()) % board.width(),
//                     (next.y + board.height()) % board.height()};
//             while (board[next] == ' ') {
//                 next += step;
//                 next = {(next.x + board.width()) % board.width(),
//                         (next.y + board.height()) % board.height()};
//             }
//             if (board[next] == '#') {
//                 break;
//             }
//             out.coords = next;
//         }
//     }
//     return out;
// }

aoc::solution_result day22(std::string_view input)
{
    const auto lines{sv_lines(input) | r::to<std::vector>};
    const auto moves{parse_moves(lines.back())};
    const auto board_lines{lines | rv::take(lines.size() - 2)};
    const auto board_width{static_cast<int>(r::max(
        board_lines | rv::transform([](auto line) { return line.size(); })))};
    const auto board_height{static_cast<int>(board_lines.size())};

    const auto pad_line{[board_width](std::string_view line) {
        return rv::concat(line, rv::repeat(' ')) | rv::take(board_width);
    }};

    grid_t board{board_width, board_height};
    r::copy(board_lines | rv::transform(pad_line) | rv::join,
            board.data().data());

    // for (const auto& row : board.rows()) {
    //     fmt::print("{}\n", row | r::to<std::string>);
    // }

    coord_t start;
    for (int x{0}; x < board_width; x++) {
        coord_t c{x, 0};
        if (board[c] == '.') {
            start = c;
            break;
        }
    }

    pos_t me{start, '>'};
    for (const auto& m : moves) {
        me = move1(board, me, m);
    }

    const auto part1_answer{score_pos(me)};

    // Part 2
    const auto edge_len{board_width > 20 ? edge_len_real : edge_len_example};

    (void)edge_pairs_example;

    return {part1_answer, edge_len};
}

}  // namespace aoc::year2022
