//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <map>
#include <string_view>
#include <utility>

namespace aoc::year2023 {

namespace {

int part1_to_int(std::string_view line)
{
    const std::string digits{line | rv::filter(is_digit) | r::to<std::string>};
    return 10 * (digits.front() - '0') + (digits.back() - '0');
}

using namespace std::literals;
const std::array<std::pair<std::string_view, int>, 19> digits_by_text{{
    {"one", 1}, {"two", 2},   {"three", 3}, {"four", 4}, {"five", 5},
    {"six", 6}, {"seven", 7}, {"eight", 8}, {"nine", 9}, {"0", 0},
    {"1", 1},   {"2", 2},     {"3", 3},     {"4", 4},    {"5", 5},
    {"6", 6},   {"7", 7},     {"8", 8},     {"9", 9}}};

int part2_find_first_digit(std::string_view line)
{
    while (line.size() != 0) {
        for (const auto& [key, value] : digits_by_text) {
            if (line.starts_with(key)) {
                return value;
            }
        }
        line = line.substr(1);
    }

    throw input_error(fmt::format("First digit not found in line: {}", line));
}

int part2_find_last_digit(std::string_view line)
{
    while (line.size() != 0) {
        for (const auto& [key, value] : digits_by_text) {
            if (line.ends_with(key)) {
                return value;
            }
        }
        line = line.substr(0, line.size() - 1);
    }

    throw input_error(fmt::format("Last digit not found in line: {}", line));
}

int part2_to_int(std::string_view line)
{
    return 10 * part2_find_first_digit(line) + part2_find_last_digit(line);
}

}  // namespace

aoc::solution_result day01(std::string_view input)
{
    const std::vector lines{sv_lines(trim(input)) | r::to<std::vector>};
    int part1{r::accumulate(lines | rv::transform(part1_to_int), 0)};
    int part2{r::accumulate(lines | rv::transform(part2_to_int), 0)};
    return {part1, part2};
}

}  // namespace aoc::year2023
