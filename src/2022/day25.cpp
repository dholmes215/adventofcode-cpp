//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace aoc::year2022 {

namespace {

using int_t = std::int64_t;

int_t char_to_int(char c)
{
    switch (c) {
        case '=':
            return -2;
        case '-':
            return -1;
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
    }
    throw input_error(fmt::format("Unrecognized character: '{}'", c));
}

int_t snafu_to_int(std::string_view snafu)
{
    const auto func{[](int_t acc, int_t i) { return acc * 5 + i; }};
    return r::accumulate(snafu | rv::transform(char_to_int), int_t{0}, func);
}

char int_to_char(int_t i)
{
    switch (i) {
        case -2:
            return '=';
        case -1:
            return '-';
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
    }
    throw new input_error(fmt::format("Unsupported 'digit': {}", i));
}

std::string int_to_snafu(int_t i)
{
    if (i == 0) {
        return "0";
    }
    std::string out;
    while (i > 0) {
        out.push_back(int_to_char((i + 2) % 5 - 2));
        i = (i + 2) / 5;
    }
    r::reverse(out);
    return out;
}

}  // namespace

aoc::solution_result day25(std::string_view input)
{
    const int_t sum{r::accumulate(
        sv_lines(trim(input)) | rv::transform(snafu_to_int), int_t{0})};
    const std::string snafu{int_to_snafu(sum)};

    return {snafu, "🎄"};
}

}  // namespace aoc::year2022
