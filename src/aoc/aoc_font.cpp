//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "aoc.hpp"
#include "aoc_font.hpp"
#include "aoc_range.hpp"

#include <array>
#include <string_view>

namespace aoc {

namespace {

constexpr std::string_view letter_a = R"(
.##.
#..#
#..#
####
#..#
#..#
)";

constexpr std::string_view letter_f = R"(
####
#...
###.
#...
#...
#...
)";

constexpr std::string_view letter_k = R"(
#..#
#.#.
##..
#.#.
#.#.
#..#
)";

constexpr std::string_view letter_r = R"(
###.
#..#
#..#
###.
#.#.
#..#
)";

constexpr std::string_view letter_u = R"(
#..#
#..#
#..#
#..#
#..#
.##.
)";

constexpr std::string_view letter_z = R"(
####
...#
..#.
.#..
#...
####
)";

std::array<char, 24> to_array(std::string_view s)
{
    std::array<char, 24> out;
    r::copy(s | rv::filter([](char c) { return c == '.' || c == '#'; }),
            out.begin());
    return out;
}

const std::map<std::array<char, 24>, char> char_lookup_map{
    {to_array(letter_a), 'A'}, {to_array(letter_f), 'F'},
    {to_array(letter_k), 'K'}, {to_array(letter_r), 'R'},
    {to_array(letter_u), 'U'}, {to_array(letter_z), 'Z'},
};

} // namespace

char recognize_char(std::span<const char, 24> data)
{
    std::array<char, 24> as_array;
    r::copy(data, as_array.begin());
    return char_lookup_map.at(as_array);
}

}  // namespace aoc
