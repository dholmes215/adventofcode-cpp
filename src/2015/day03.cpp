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
        r::find_if(input, [=](const char c) { return !valid(c); })};
    const auto invalid =
        std::find_if(end_of_input, input.end(),
                     [](const char c) { return !is_whitespace(c); });
    if (invalid != input.end()) {
        throw input_error("Invalid input.");
    }
    return r::subrange{input.begin(), end_of_input};
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
                     rv::transform(char_to_move) | r::to<std::vector>()};
    auto visited_houses{moves | rv::partial_sum | r::to<std::vector>()};
    r::sort(visited_houses);
    const auto unique_houses{
        r::subrange(visited_houses.begin(), r::unique(visited_houses))};

    const auto visited_by_santa{moves | rv::stride(2) | rv::partial_sum |
                                r::to<std::vector>()};
    const auto visited_by_robo{moves | rv::drop(1) | rv::stride(2) |
                               rv::partial_sum | r::to<std::vector>()};

    auto visited_by_santa_or_robo{
        rv::concat(visited_by_santa, visited_by_robo) | r::to<std::vector>()};
    r::sort(visited_by_santa_or_robo);
    const auto unique_houses_b{r::subrange(
        visited_by_santa_or_robo.begin(), r::unique(visited_by_santa_or_robo))};

    return {r::distance(unique_houses), r::distance(unique_houses_b)};
}

}  // namespace aoc::year2015
