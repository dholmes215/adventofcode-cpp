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
#include <vector>

namespace aoc::year2015 {

aoc::solution_result day01(std::istream& input)
{
    int floor{0};
    int position{1};
    std::optional<int> first_position_in_basement{};
    for (const char c : istream_range{input}) {
        if (c == '(') {
            floor++;
        }
        else if (c == ')') {
            floor--;
        }
        else if (!is_whitespace(c)) {
            fmt::print(stderr, "Read unexpected input character '{}'.\n", c);
        }
        if (floor < 0 && !first_position_in_basement) {
            first_position_in_basement = position;
        }
        position++;
    }

    return {std::to_string(floor), std::to_string(*first_position_in_basement)};
}

// int process_input(const char c)
// {
//     if (c == '(') {
//         return 1;
//     }
//     else if (c == ')') {
//         return -1;
//     }
//     else if (!is_whitespace(c)) {
//         fmt::print(stderr, "Read unexpected input character '{}'.\n", c);
//     }
//     return 0;
// }

int process_input(const char c)
{
    return (c - '(') * -2 + 1;
}

aoc::solution_result day01algorithm(std::istream& input)
{
    const auto input_range{istream_range{input}};
    std::vector<int> moves;
    std::transform(input_range.begin(), input_range.end(),
                   std::back_inserter(moves), process_input);
    const auto begin = moves.begin();
    const auto end = moves.end();
    // const auto sum{std::accumulate(begin, end, 0)};

    // Overwrite the input range with the sum of the moves.
    std::partial_sum(begin, end, begin);
    const auto found = std::find(begin, end, -1);
    const auto first_position_in_basement{1 + found - begin};

    return {std::to_string(moves.back()),
            std::to_string(first_position_in_basement)};
}

aoc::solution_result day01ranges(std::istream& input)
{
    using ranges::views::enumerate;
    using ranges::views::partial_sum;
    using ranges::views::transform;

    const auto input_range{istream_range{input}};
    // const auto input_range{
    //     ranges::subrange{std::istreambuf_iterator<char>{input},
    //                      std::istreambuf_iterator<char>{}}};
    // TODO: Why is subrange slow?
    // TODO: Use const char* anyways.  Or std::string_view?  Or iterator
    // template parameters?
    // TODO: Run multiple iterations and benchmark them

    // const ranges::istream_view<char> input_range{input};
    const auto sums_range =
        input_range | transform(process_input) | partial_sum;
    const auto counted = ranges::views::enumerate(sums_range);

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