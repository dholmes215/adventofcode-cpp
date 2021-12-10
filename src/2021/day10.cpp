//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <map>
#include <numeric>
#include <string_view>

namespace aoc::year2021 {

namespace {

}  // namespace

const std::map<char, int> syntax_points{{')', 3},
                                        {']', 57},
                                        {'}', 1197},
                                        {'>', 25137}};

const std::map<char, std::uint64_t> autocomplete_points{{')', 1ULL},
                                                        {']', 2ULL},
                                                        {'}', 3ULL},
                                                        {'>', 4ULL}};

const std::map<char, char> left_to_right{{'(', ')'},
                                         {'[', ']'},
                                         {'{', '}'},
                                         {'<', '>'}};

struct line_scores {
    int syntax_error_score_{0};
    std::uint64_t autocomplete_score_{0};
};

int to_syntax_error_score(const line_scores& scores)
{
    return scores.syntax_error_score_;
}

std::uint64_t to_autocomplete_score(const line_scores& scores)
{
    return scores.autocomplete_score_;
}

line_scores line_points(std::string_view line)
{
    std::string stack;
    stack.reserve(line.size());

    // Syntax points
    for (auto c : line) {
        if (left_to_right.contains(c)) {
            stack.push_back(c);
        }
        else if (stack.empty()) {
            if (!syntax_points.contains(c)) {
                throw input_error{fmt::format("Unexpected character '{}'", c)};
            }
            return {syntax_points.at(c), 0};
        }
        else {
            char left = stack.back();
            if (left_to_right.at(left) == c) {
                stack.pop_back();
            }
            else {
                return {syntax_points.at(c), 0};
            }
        }
    }

    // Autocomplete points
    if (!stack.empty()) {
        auto flip{[](char c) { return left_to_right.at(c); }};
        r::reverse(stack);
        r::transform(stack, stack.begin(), flip);
        auto accumulate_points{[](std::uint64_t points, char c) {
            return points * 5 + autocomplete_points.at(c);
        }};
        const auto points{r::accumulate(stack, 0ULL, accumulate_points)};
        return {0, points};
    }
    throw input_error{fmt::format("input line was apparently valid: {}", line)};
}

aoc::solution_result day10(std::string_view input)
{
    const auto lines{sv_lines(input) | r::to<std::vector>};
    const auto scores{lines | rv::transform(line_points)};

    const auto total_syntax_score{
        r::accumulate(scores | rv::transform(to_syntax_error_score), 0)};

    auto autocomplete_scores{scores | rv::transform(to_autocomplete_score) |
                             rv::remove(0ULL) | r::to<std::vector>};
    r::sort(autocomplete_scores);
    const std::uint64_t middle_score{
        autocomplete_scores[autocomplete_scores.size() / 2]};

    return {total_syntax_score, middle_score};
}

}  // namespace aoc::year2021
