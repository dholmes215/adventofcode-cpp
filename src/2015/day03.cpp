//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <algorithm>
#include <bitset>
#include <string_view>
#include <vector>

namespace aoc::year2015 {

using Scalar = int;
using Vec2 = vec2<Scalar>;

namespace {

bool char_is_valid(char c)
{
    static const std::bitset<128> valid_chars{[]() {
        std::bitset<128> bs;
        bs.set('<');
        bs.set('>');
        bs.set('^');
        bs.set('v');
        return bs;
    }()};
    return valid_chars.test(static_cast<size_t>(c));
}

using char_predicate = decltype(char_is_valid);

// Input should be only "v^<>", followed by only whitespace.
auto validate_input(std::string_view input, char_predicate valid)
{
    const auto end_of_input{
        find_if(input, [=](const char c) { return !valid(c); })};
    const auto invalid =
        std::find_if(end_of_input, input.end(),
                     [](const char c) { return !is_whitespace(c); });
    if (invalid != input.end()) {
        throw input_error("Invalid input.");
    }
    return subrange{input.begin(), end_of_input};
}

Vec2 char_to_move(char c)
{
    switch (c) {
        case '^':
            return {0, -1};
        case 'v':
            return {0, 1};
        case '<':
            return {-1, 0};
        case '>':
            return {1, 0};
        default:
            std::abort();  // Unreachable if input was validated
    }
}

}  // namespace

aoc::solution_result day03(std::string_view input)
{
    const auto moves{validate_input(input, char_is_valid) |
                     transform(char_to_move) | to<std::vector>()};
    auto visited_houses{moves | partial_sum | to<std::vector>()};
    sort(visited_houses);
    const auto unique_houses{
        subrange(visited_houses.begin(), unique(visited_houses))};

    const auto visited_by_santa{moves | stride(2) | partial_sum |
                                to<std::vector>()};
    const auto visited_by_robo{moves | drop(1) | stride(2) | partial_sum |
                               to<std::vector>()};

    auto visited_by_santa_or_robo{concat(visited_by_santa, visited_by_robo) |
                                  to<std::vector>()};
    sort(visited_by_santa_or_robo);
    const auto unique_houses_b{subrange(visited_by_santa_or_robo.begin(),
                                        unique(visited_by_santa_or_robo))};

    return {distance(unique_houses), distance(unique_houses_b)};
}

}  // namespace aoc::year2015
