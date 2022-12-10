//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_FONT_HPP
#define AOC_FONT_HPP

#include "aoc_grid.hpp"
#include "aoc_range.hpp"

#include <array>
#include <span>
#include <stdexcept>

namespace aoc {

char recognize_char(std::span<const char, 30> data);

// TODO: Concepts to constrain all this

// Expects '#' and '.' as character values
// TODO: Something more general
char recognize_char_rng(auto&& data_rng)
{
    std::array<char, 30> char_array;
    r::copy(data_rng | rv::take(30), char_array.begin());
    return recognize_char(char_array);
}

// Expects '#' and '.' as character values
// TODO: Something more general
std::string recognize_string_grid(const auto& grid)
{
    std::string out;
    for (int i{0}; i < grid.width() / 5; i++) {
        out += recognize_char_rng(grid.subgrid({{i * 5, 0}, {5, 6}}).data());
    }
    return out;
}

// Expects '#' and '.' as character values
// TODO: Something more general
// TODO: Don't depend on grid
template <int Chars, typename Rng>
std::string recognize_string_rng(Rng&& pixel_rng)
{
    static_grid<char, Chars * 5, 6> grid;
    r::copy(pixel_rng, grid.data().data());
    return recognize_string_grid(grid);
}

// Exception thrown when input character data cannot be recognized.
class font_error : public std::domain_error {
   public:
    font_error(std::string msg) : domain_error{std::move(msg)} {}
};

}  // namespace aoc

#endif  // AOC_FONT_HPP
