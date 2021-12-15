//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_BRAILLE_HPP
#define AOC_BRAILLE_HPP

#include "aoc_vec.hpp"

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <span>
#include <string>

namespace aoc {

using codepoint_t = std::uint32_t;

std::uint8_t pack_bools(std::span<const bool, 8> bools);

codepoint_t to_braille(std::span<const bool, 4> bit_col_1,
                       std::span<const bool, 4> bit_col_2);

std::string to_braille_utf8(std::span<const bool, 4> bit_col_1,
                            std::span<const bool, 4> bit_col_2);

std::array<bool, 4> to_bit_col(bool a, bool b, bool c, bool d);

std::string braille_codepoint_to_utf8(codepoint_t codepoint);

void print_grid_braille(const auto& grid, auto to_bool_func)
{
    auto point_to_dot{[&](vec2<int> p) {
        return grid.area().contains(p) ? to_bool_func(grid[p]) : false;
    }};

    for (int row{0}; row < grid.height(); row += 4) {
        for (int col{0}; col < grid.width(); col += 2) {
            std::array<bool, 4> bit_col1{point_to_dot({col + 0, row + 0}),
                                         point_to_dot({col + 0, row + 1}),
                                         point_to_dot({col + 0, row + 2}),
                                         point_to_dot({col + 0, row + 3})};
            std::array<bool, 4> bit_col2{point_to_dot({col + 1, row + 0}),
                                         point_to_dot({col + 1, row + 1}),
                                         point_to_dot({col + 1, row + 2}),
                                         point_to_dot({col + 1, row + 3})};

            const auto braille_str{to_braille_utf8(bit_col1, bit_col2)};
            fmt::print("{}", braille_str);
        }
        fmt::print("\n");
    }
}

}  // namespace aoc

#endif  // AOC_BRAILLE_HPP
