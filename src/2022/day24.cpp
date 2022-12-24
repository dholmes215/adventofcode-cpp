//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_graph.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <fmt/format.h>

#include <array>
#include <bit>
#include <cstdint>
#include <string_view>
#include <string>
#include <vector>

namespace aoc::year2022 {

namespace {

class tile_t {
   public:
    tile_t() noexcept : data_{0} {}
    tile_t(char input_char) noexcept : data_{0}  // deliberately implicit
    {
        switch (input_char) {
            case '^':
                data_ = up_;
                break;
            case 'v':
                data_ = down_;
                break;
            case '<':
                data_ = left_;
                break;
            case '>':
                data_ = right_;
                break;
            case '.':
                data_ = 0;
                break;
            case 'E':
                // This isn't actually in the input file, but handle it here so
                // we can conveniently insert it in the input.
                data_ = us_;
                break;
            case '#':
                data_ = wall_;
                break;
        }
    }
    tile_t(bool up2, bool down2, bool left2, bool right2) noexcept
        : data_{static_cast<std::uint8_t>(
              (up2 ? up_ : 0) | (down2 ? down_ : 0) | (left2 ? left_ : 0) |
              (right2 ? right_ : 0))}
    {
    }

    // tile_t(const tile_t&) = default;
    // tile_t& operator=(const tile_t&) = default;

    bool up() const noexcept { return data_ & up_; }
    bool down() const noexcept { return data_ & down_; }
    bool left() const noexcept { return data_ & left_; }
    bool right() const noexcept { return data_ & right_; }
    bool us() const noexcept { return data_ & us_; }
    bool wall() const noexcept { return data_ & wall_; }
    bool empty() const noexcept { return data_ == 0; }

    char draw() const noexcept
    {
        if (wall()) {
            return '#';
        }
        if (us()) {
            return 'E';
        }
        if (empty()) {
            return '.';
        }
        const auto blizzards{
            std::popcount(static_cast<std::uint8_t>(data_ & blizzards_))};
        if (blizzards == 1) {
            if (up()) {
                return '^';
            }
            if (down()) {
                return 'v';
            }
            if (left()) {
                return '<';
            }
            if (right()) {
                return '>';
            }
        }
        return '0' + static_cast<char>(blizzards);
    }

   private:
    static const std::uint8_t up_{0b000001};
    static const std::uint8_t down_{0b000010};
    static const std::uint8_t left_{0b000100};
    static const std::uint8_t right_{0b001000};
    static const std::uint8_t us_{0b010000};
    static const std::uint8_t wall_{0b100000};
    static const std::uint8_t blizzards_{0b001111};
    std::uint8_t data_;
};

using grid_t = dynamic_grid<tile_t>;
using pos_t = vec2<int>;

const pos_t start_pos{1, 0};

const std::array<pos_t, 5> moves{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {0, 0}}};

grid_t parse_grid(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    grid_t grid{static_cast<int>(lines[0].size()),
                static_cast<int>(lines.size())};
    const auto tiles{lines | rv::join |
                     rv::transform([](char c) { return tile_t{c}; })};
    r::copy(tiles, grid.data().data());
    // grid[start_pos] = 'E';
    return grid;
}

// void print_grid(const grid_t& grid)
// {
//     for (const auto& row : grid.rows()) {
//         fmt::print("{}\n", row | rv::transform([](tile_t t) {
//                                return t.draw();
//                            }) | r::to<std::string>);
//     }
// }

grid_t move_blizzards(const grid_t& grid)
{
    grid_t out{grid};
    auto sub_in{grid.subgrid({{1, 1}, {grid.width() - 2, grid.height() - 2}})};
    auto sub_out{out.subgrid({{1, 1}, {grid.width() - 2, grid.height() - 2}})};

    const pos_t dims{sub_out.width(), sub_in.height()};
    const auto wrap_pos{[dims](pos_t p) {
        return pos_t{(p.x + dims.x) % dims.x, (p.y + dims.y) % dims.y};
    }};

    for (const pos_t& pos : sub_out.area().all_points()) {
        pos_t up{wrap_pos(pos + pos_t{0, -1})};
        pos_t down{wrap_pos(pos + pos_t{0, 1})};
        pos_t left{wrap_pos(pos + pos_t{-1, 0})};
        pos_t right{wrap_pos(pos + pos_t{1, 0})};
        sub_out[pos] = tile_t{sub_in[down].up(), sub_in[up].down(),
                              sub_in[right].left(), sub_in[left].right()};
    }

    return out;
}

struct state_t {
    int minute{0};
    pos_t us{start_pos};
    friend auto operator<=>(const state_t&, const state_t&) noexcept = default;
};

}  // namespace

aoc::solution_result day24(std::string_view input)
{
    auto grid{parse_grid(input)};

    // The movement of the blizzards over time is completely independent of any
    // of our own moves, so instead of computing it as part of the state
    // transition, just compute them all up front.  In our own input the
    // movements will cycle after 600 minutes, so we don't need more than that
    // TODO: Don't hardcode 600; the number will be the least common multiple of
    // the width and height of the subgrid.
    // TODO Check is this really right?
    std::vector<grid_t> grids;
    grids.push_back(grid);
    while (grids.size() < 600) {
        grids.push_back(move_blizzards(grids.back()));
    }

    const auto grids_cycle{grids | rv::cycle};

    const auto adj_func{[&](const state_t& s) {
        int next_minute{s.minute + 1};
        const auto& next_grid{grids_cycle[next_minute]};

        const auto can_move{[&](const state_t& s2) {
            return next_grid.area().contains(s2.us) &&
                   (next_grid[s2.us].empty());
        }};

        return moves | rv::transform([s, next_minute](pos_t d) {
                   return state_t{next_minute, d + s.us};
               }) |
               rv::filter(can_move);
    }};

    const pos_t end_pos{grid.width() - 2, grid.height() - 1};
    const auto accept_func1{[&](const state_t& s) { return s.us == end_pos; }};
    const auto path1{bfs_accept(adj_func, state_t{}, accept_func1)};

    const auto accept_func2{
        [&](const state_t& s) { return s.us == start_pos; }};
    const auto path2{bfs_accept(adj_func, path1.back(), accept_func2)};

    const auto path3{bfs_accept(adj_func, path2.back(), accept_func1)};

    return {path1.back().minute, path3.back().minute};
}

}  // namespace aoc::year2022
