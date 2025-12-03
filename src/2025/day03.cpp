//
// Copyright (c) 2025 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <string_view>

namespace aoc::year2025 {

namespace {

using joltage = std::uint64_t;

joltage digit_to_joltage(const char c)
{
    return c - '0';
}

std::size_t find_digit_distance(const std::string_view line)
{
    return r::distance(r::begin(line),
                       r::max_element(line));
}

constexpr std::int64_t pow_of_10(const std::uint64_t n)
{
    return n == 0 ? 1 : 10 * pow_of_10(n - 1);
}

joltage find_joltage(const std::string_view line, const std::size_t digit_count)
{
    assert(digit_count > 0);
    if (digit_count == 1) {
        return digit_to_joltage(*r::max_element(line));
    }

    const auto first_digit_distance = find_digit_distance(
        line.substr(0, line.size() - digit_count + 1));

    return (digit_to_joltage(line[first_digit_distance]) *
            pow_of_10(digit_count - 1)) +
           find_joltage(line.substr(first_digit_distance + 1), digit_count - 1);
}

joltage sum_joltage(auto&& lines, const std::size_t digit_count)
{
    return r::accumulate(lines | rv::transform([digit_count](std::string_view line) {
                               return find_joltage(line, digit_count);
                           }),
                         joltage{0});
}

}  // namespace

aoc::solution_result day03(std::string_view input)
{
    input = trim(input);
    auto lines = sv_lines(input);
    auto a = sum_joltage(lines, 2);;
    auto b = sum_joltage(lines, 12);;

    return {a, b};
}

}  // namespace aoc::year2025
