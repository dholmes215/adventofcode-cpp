//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_font.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <tiny_vector.hpp>

#include <array>
#include <functional>
#include <string_view>
#include <vector>

namespace aoc::year2022 {

namespace {

tiny_vector<char> line_to_cycle_addends(std::string_view line)
{
    tiny_vector<char> out;
    out.push_back('\0');
    if (line.front() != 'n') {
        out.push_back(to_num<char>(line.substr(5)));
    }
    return out;
}

const auto signal_strength{[](auto pair) {
    auto [cycle, x]{pair};
    return static_cast<int>(cycle) * x;
}};

const auto pixel_on{[](auto pair) {
    auto [cycle, x_reg]{pair};
    int crt_pos{(static_cast<int>(cycle) - 1) % 40};
    return std::abs(x_reg - crt_pos) <= 1;
}};

char bool_to_char(bool b)
{
    return b ? '#' : '.';
}

}  // namespace

aoc::solution_result day10(std::string_view input)
{
    const std::array<int, 2> initial_values{0, 1};
    const auto addends{
        rv::concat(initial_values,
                   sv_lines(trim(input)) |
                       rv::transform(line_to_cycle_addends) | rv::join |
                       rv::transform([](char c) { return int{c}; })) |
        r::to<std::vector>};

    const auto x_register{addends | rv::partial_sum};

    const int signal_sum{r::accumulate(x_register | rv::enumerate |
                                           rv::transform(signal_strength) |
                                           rv::drop(20) | rv::stride(40),
                                       0)};

    static_grid<char, 40, 6> grid;
    r::copy(x_register | rv::enumerate | rv::drop(1) | rv::transform(pixel_on) |
                rv::transform(bool_to_char),
            grid.data().data());

    std::string message;
    for (int i{0}; i < 8; i++) {
        std::array<char, 30> char_array;
        r::copy(grid.subgrid({{i * 5, 0}, {5, 6}}).data(), char_array.begin());
        message += recognize_char(char_array);
    }

    return {signal_sum, message};
}

}  // namespace aoc::year2022
