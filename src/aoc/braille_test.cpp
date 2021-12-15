//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "aoc_braille.hpp"

#include <fmt/format.h>

#include <array>

int main()
{
    std::array<bool, 4> col1{false, false, false, true};
    std::array<bool, 4> col2{false, false, true, true};
    std::array<bool, 4> col3{false, true, true, true};
    std::array<bool, 4> col4{true, true, true, false};

    aoc::codepoint_t cp1{aoc::to_braille(col1, col2)};
    aoc::codepoint_t cp2{aoc::to_braille(col3, col4)};
    fmt::print("{}{}\n", aoc::braille_codepoint_to_utf8(cp1),
               aoc::braille_codepoint_to_utf8(cp2));
}