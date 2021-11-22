//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>

#include <range/v3/all.hpp>

#include <fmt/core.h>

#include <algorithm>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

namespace aoc::year2015 {

namespace {
// Input should be only '(' and ')', followed by only whitespace.
auto validate_input(std::string_view input)
{
    const auto end_of_input{ranges::find_if(
        input, [](const char c) { return !(c == ')' || c == '('); })};
    const auto invalid =
        std::find_if(end_of_input, input.end(),
                     [](const char c) { return !is_whitespace(c); });
    if (invalid != input.end()) {
        throw input_error("Invalid input.");
    }
    return ranges::subrange{input.begin(), end_of_input};
}

// Convert '(' and ')' to -1 or 1.
int process_input(const char c)
{
    static_assert('(' + 1 == ')');
    return (c - '(') * -2 + 1;
}
}  // namespace

aoc::solution_result day01(std::string_view input)
{
    const auto valid_input{validate_input(input)};
    int floor{0};

    auto iter{valid_input.begin()};
    while (floor >= 0) {
        const char c = *iter++;

        // if (c == '(') {
        //     floor++;
        // }
        // else {
        //     floor--;
        // }

        // By first validating that input contains only '(' and ')', we can
        // eliminate the need for branches for each character, considerably
        // improving performance.
        floor += process_input(c);
    }

    const auto first_position_in_basement{iter - valid_input.begin()};

    while (iter != valid_input.end()) {
        const char c = *iter++;
        floor += process_input(c);
    }

    return {std::to_string(floor), std::to_string(first_position_in_basement)};
}

aoc::solution_result day01algorithm(std::string_view input)
{
    const auto valid_input{validate_input(input)};
    // std::vector<int> moves;
    // std::transform(valid_input.begin(), valid_input.end(),
    //                std::back_inserter(moves), process_input);
    // Using std::back_inserter() instead of pre-sizing the vector dramatically
    // worsens performance.
    std::vector<int> moves(input.size());
    std::transform(valid_input.begin(), valid_input.end(), moves.begin(),
                   process_input);
    const auto begin = moves.begin();
    const auto end = moves.end();

    // Overwrite the input range with the sum of the moves.
    std::partial_sum(begin, end, begin);
    const auto found = std::find(begin, end, -1);
    const auto first_position_in_basement{1 + found - begin};

    return {std::to_string(moves.back()),
            std::to_string(first_position_in_basement)};
}

aoc::solution_result day01ranges(std::string_view input)
{
    using ranges::views::enumerate;
    using ranges::views::partial_sum;
    using ranges::views::transform;

    const auto counted = validate_input(input) | transform(process_input) |
                         partial_sum | enumerate;
    const auto found = ranges::find_if(
        counted, [](const auto& pair) { return pair.second == -1; });

    const auto basement_pair = *found;
    const auto first_position_in_basement = basement_pair.first + 1;

    const auto rest_of_range = ranges::subrange{found, ranges::end(counted)};
    // const auto last_pair = rest_of_range | ranges::views::take_last(1);
    int last_floor{0};
    for (const auto& pair : rest_of_range) {
        last_floor = pair.second;
    }

    return {std::to_string(last_floor),
            std::to_string(first_position_in_basement)};
}

}  // namespace aoc::year2015
