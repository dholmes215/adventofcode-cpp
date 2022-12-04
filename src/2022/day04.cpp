//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <ctre.hpp>

#include <fmt/format.h>

#include <string_view>
#include <utility>

namespace aoc::year2022 {

namespace {

using number_t = std::uint8_t;
using range_t = std::pair<number_t, number_t>;
using pair_t = std::pair<range_t, range_t>;

pair_t parse_line(std::string_view line)
{
    constexpr auto matcher{ctre::match<R"((\d+)-(\d+),(\d+)-(\d+))">};
    if (auto [whole, a1, a2, b1, b2] = matcher(line); whole) {
        return {{to_num<number_t>(a1), to_num<number_t>(a2)},
                {to_num<number_t>(b1), to_num<number_t>(b2)}};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

constexpr bool contains(range_t a, range_t b)
{
    return a.first <= b.first && a.second >= b.second;
}

constexpr bool either_contains(pair_t pair)
{
    return contains(pair.first, pair.second) ||
           contains(pair.second, pair.first);
}

constexpr bool contains_end(range_t a, range_t b)
{
    return (a.first >= b.first && a.first <= b.second) ||
           (b.first >= a.first && b.first <= a.second);
}

constexpr bool overlaps(pair_t pair)
{
    return contains_end(pair.first, pair.second) ||
           contains_end(pair.second, pair.first);
}

}  // namespace

aoc::solution_result day04(std::string_view input)
{
    const auto pairs{sv_lines(trim(input)) | rv::transform(parse_line) |
                     r::to<std::vector>};

    return {r::count_if(pairs, either_contains), r::count_if(pairs, overlaps)};
}

}  // namespace aoc::year2022
