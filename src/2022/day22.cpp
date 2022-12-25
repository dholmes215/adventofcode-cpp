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
#include <coro_generator.hpp>

#include <fmt/format.h>

#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace aoc::year2022 {

namespace {

using turn_t = char;    // 'L' or 'R'
using facing_t = char;  // '<' or '>' or '^' or 'v'
using dist_t = int;
using grid_t = dynamic_grid<char>;
using coord_t = vec2<int>;
using move_t = std::variant<dist_t, turn_t>;

struct pos_t {
    coord_t coords;
    facing_t facing;
    friend auto operator<=>(const pos_t&, const pos_t&) noexcept = default;
};

const int edge_len_example{4};
const int edge_len_real{50};

using side_t = vec2<int>;
enum class corner_t { nw, ne, se, sw };

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

char facing_reverse(facing_t f)
{
    switch (f) {
        case '>':
            return '<';
        case 'v':
            return '^';
        case '<':
            return '>';
        case '^':
            return 'v';
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
                {{3, 2}, corner_t::se, corner_t::ne}},
    edge_pair_t{{{2, 1}, corner_t::ne, corner_t::se},
                {{3, 2}, corner_t::ne, corner_t::nw}},
    edge_pair_t{{{3, 2}, corner_t::se, corner_t::sw},
                {{0, 1}, corner_t::nw, corner_t::sw}},
    edge_pair_t{{{2, 2}, corner_t::sw, corner_t::se},
                {{0, 1}, corner_t::se, corner_t::sw}},
    edge_pair_t{{{2, 2}, corner_t::sw, corner_t::nw},
                {{1, 1}, corner_t::sw, corner_t::se}}};

const std::array edge_pairs_real{
    edge_pair_t{{{1, 0}, corner_t::nw, corner_t::sw},
                {{0, 2}, corner_t::sw, corner_t::nw}},
    edge_pair_t{{{1, 0}, corner_t::nw, corner_t::ne},
                {{0, 3}, corner_t::nw, corner_t::sw}},
    edge_pair_t{{{2, 0}, corner_t::nw, corner_t::ne},
                {{0, 3}, corner_t::sw, corner_t::se}},
    edge_pair_t{{{2, 0}, corner_t::ne, corner_t::se},
                {{1, 2}, corner_t::se, corner_t::ne}},
    edge_pair_t{{{2, 0}, corner_t::se, corner_t::sw},
                {{1, 1}, corner_t::se, corner_t::ne}},
    edge_pair_t{{{1, 2}, corner_t::se, corner_t::sw},
                {{0, 3}, corner_t::se, corner_t::ne}},
    edge_pair_t{{{0, 2}, corner_t::nw, corner_t::ne},
                {{1, 1}, corner_t::nw, corner_t::sw}}};

facing_t facing_from_corners(corner_t a, corner_t b)
{
    std::array arr{a, b};
    r::sort(arr);
    if (arr == std::array{corner_t::nw, corner_t::ne}) {
        return '^';
    }
    if (arr == std::array{corner_t::ne, corner_t::se}) {
        return '>';
    }
    if (arr == std::array{corner_t::se, corner_t::sw}) {
        return 'v';
    }
    if (arr == std::array{corner_t::nw, corner_t::sw}) {
        return '<';
    }
    throw input_error("Invalid combination of corners");
}

Generator<coord_t> coord_line(coord_range_t r)
{
    coord_t delta{r.last - r.first};
    coord_t step{delta.x == 0 ? 0 : delta.x / std::abs(delta.x),
                 delta.y == 0 ? 0 : delta.y / std::abs(delta.y)};
    coord_t pos(r.first);
    while (pos != r.last) {
        co_yield pos;
        pos += step;
    }
    co_yield pos;
}

std::map<pos_t, pos_t> build_edge_map(
    const std::span<const edge_pair_t> edge_pairs,
    int edge_len)
{
    std::map<pos_t, pos_t> out;
    std::vector<coord_t> coord_line_a;
    std::vector<coord_t> coord_line_b;
    for (const auto& [a, b] : edge_pairs) {
        auto [side_a, corner1_a, corner2_a]{a};
        auto [side_b, corner1_b, corner2_b]{b};
        facing_t f_a{facing_from_corners(corner1_a, corner2_a)};
        facing_t f_b{facing_from_corners(corner1_b, corner2_b)};

        coord_t step_a{facing_step(f_a)};
        coord_t step_b{facing_step(f_b)};
        coord_t coord1_a{side_corner_pos(side_a, corner1_a, edge_len)};
        coord_t coord2_a{side_corner_pos(side_a, corner2_a, edge_len)};

        coord_t coord1_b{side_corner_pos(side_b, corner1_b, edge_len)};
        coord_t coord2_b{side_corner_pos(side_b, corner2_b, edge_len)};

        coord_line_a.clear();
        coord_line_b.clear();
        for (coord_t c : coord_line({coord1_a, coord2_a})) {
            coord_line_a.push_back(c);
        }
        for (coord_t c : coord_line({coord1_b, coord2_b})) {
            coord_line_b.push_back(c);
        }

        for (const auto& [ca, cb] : rv::zip(coord_line_a, coord_line_b)) {
            out[{ca + step_a, f_a}] = {cb, facing_reverse(f_b)};
            out[{cb + step_b, f_b}] = {ca, facing_reverse(f_a)};
        }
    }
    return out;
}

const auto edge_map_example{
    build_edge_map(edge_pairs_example, edge_len_example)};

const auto edge_map_real{build_edge_map(edge_pairs_real, edge_len_real)};

pos_t move2(const grid_t& board,
            pos_t p,
            const move_t& m,
            const std::map<pos_t, pos_t>& edge_map)
{
    pos_t out{p};
    if (const turn_t* maybe_turn = std::get_if<turn_t>(&m)) {
        out.facing = turn(out.facing, *maybe_turn);
    }
    else {
        dist_t dist{std::get<dist_t>(m)};
        auto step{facing_step(out.facing)};
        for (int i{0}; i < dist; i++) {
            auto next{out};
            next.coords += step;
            if (!board.area().contains(next.coords) ||
                board[next.coords] == ' ') {
                next = edge_map.at(next);
                step = facing_step(next.facing);
            }
            if (board[next.coords] == '#') {
                break;
            }
            out = next;
        }
    }
    return out;
}

}  // namespace

// FIXME: Need to come up with a general solution to these that's not just
// my own hardcoded input

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
    const auto edge_map{board_width > 20 ? edge_map_real : edge_map_example};

    pos_t me2{start, '>'};
    for (const auto& m : moves) {
        me2 = move2(board, me2, m, edge_map);
    }

    const auto part2_answer{score_pos(me2)};

    return {part1_answer, part2_answer};
}

}  // namespace aoc::year2022
