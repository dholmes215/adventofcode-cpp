//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <bit>
#include <string_view>

namespace aoc::year2022 {

namespace {

constexpr auto not_unique{[](const auto& rng) {
    std::uint64_t mask{0};
    for (char c : rng) {
        mask |= 1ULL << (c - 'A');
    }
    return std::popcount(mask) != r::distance(rng);
}};

}  // namespace

aoc::solution_result day06(std::string_view input)
{
    input = trim(input);

    const auto part1{
        r::distance(input | rv::sliding(4) | rv::take_while(not_unique)) + 4};
    const auto part2{
        r::distance(input | rv::sliding(14) | rv::take_while(not_unique)) + 14};

    return {part1, part2};
}

}  // namespace aoc::year2022
