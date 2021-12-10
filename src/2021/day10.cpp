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
#include <string>
#include <string_view>

namespace aoc::year2021 {

namespace {

const std::map<char, int> syntax_points{{')', 3},
                                        {']', 57},
                                        {'}', 1197},
                                        {'>', 25137}};

// const std::map<char, std::uint64_t> autocomplete_points{{')', 1ULL},
//                                                         {']', 2ULL},
//                                                         {'}', 3ULL},
//                                                         {'>', 4ULL}};

std::uint64_t autocomplete_points_fn(char c)
{
    switch (c) {
        case ')':
            return 1;
        case ']':
            return 2;
        case '}':
            return 3;
        case '>':
            return 4;
    }
    return 0;
}

// const std::map<char, char> left_to_right{{'(', ')'},
//                                          {'[', ']'},
//                                          {'{', '}'},
//                                          {'<', '>'}};

char left_to_right_fn(char c)
{
    switch (c) {
        case '(':
            return ')';
        case '[':
            return ']';
        case '{':
            return '}';
        case '<':
            return '>';
    }
    return '\0';
}

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

int line_syntax_points(std::string& stack, std::string_view line)
{
    for (auto c : line) {
        if (left_to_right_fn(c)) {
            stack.push_back(c);
        }
        else if (stack.empty()) {
            return syntax_points.at(c);
        }
        else {
            if (left_to_right_fn(stack.back()) == c) {
                stack.pop_back();
            }
            else {
                return syntax_points.at(c);
            }
        }
    }

    // TODO: consider constructing stack in function and returning a variant of
    // either a score, or the stack, depending on the result.
    return 0;
}

std::uint64_t line_autocomplete_points(std::string& stack)
{
    auto accumulate_points{[](std::uint64_t points, char c) {
        return points * 5 + autocomplete_points_fn(c);
    }};
    return r::accumulate(stack | rv::reverse | rv::transform(left_to_right_fn),
                         0ULL, accumulate_points);
}

line_scores line_points(std::string_view line)
{
    std::string stack;
    stack.reserve(line.size());

    // Syntax points
    const auto points{line_syntax_points(stack, line)};
    if (points != 0) {
        return {points, 0};
    }

    // Autocomplete points
    if (!stack.empty()) {
        return {0, line_autocomplete_points(stack)};
    }
    throw input_error{fmt::format("input line was apparently valid: {}", line)};
}

int count_syntax_scores(const std::vector<line_scores>& scores)
{
    return r::accumulate(scores | rv::transform(to_syntax_error_score), 0);
}

std::uint64_t select_autocomplete_score(const std::vector<line_scores>& scores)
{
    auto autocomplete_scores{scores | rv::transform(to_autocomplete_score) |
                             rv::remove(0ULL) | r::to<std::vector>};
    r::sort(autocomplete_scores);
    return autocomplete_scores[autocomplete_scores.size() / 2];
}

}  // namespace

aoc::solution_result day10(std::string_view input)
{
    const auto scores{sv_lines(input) | rv::transform(line_points) |
                      r::to<std::vector>};
    return {count_syntax_scores(scores), select_autocomplete_score(scores)};
}

}  // namespace aoc::year2021
