//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <string_view>
#include <utility>

namespace aoc::year2022 {

namespace {

int score_shape(const char c)
{
    switch (c) {
        case 'X':
            return 1;
        case 'Y':
            return 2;
        case 'Z':
            return 3;
    }
    throw input_error(fmt::format("Unsupported character: {}", c));
}

enum class result { win, loss, tie };

result winner(const std::pair<char, char> p)
{
    const auto [l, r]{p};
    if (l == r - 23) {
        return result::tie;
    }
    if ((l == 'A' && r == 'Z') ||  // Rock beats scissors
        (l == 'B' && r == 'X') ||  // Paper beats rock
        (l == 'C' && r == 'Y')) {  // Scissors beats paper
        return result::loss;
    }
    return result::win;
}

int score_result(const result r)
{
    switch (r) {
        case result::win:
            return 6;
        case result::tie:
            return 3;
        case result::loss:
            return 0;
    }
}

result needed_result(const char c)
{
    switch (c) {
        case 'X':
            return result::loss;
        case 'Y':
            return result::tie;
        case 'Z':
            return result::win;
    }
    throw input_error(fmt::format("Unsupported character: {}", c));
}

char select_response(const std::pair<char, char> p)
{
    const auto [them, r]{p};
    const auto desired_result{needed_result(r)};
    switch (them) {
        case 'A':
            switch (desired_result) {
                case result::loss:
                    return 'Z';
                case result::tie:
                    return 'X';
                case result::win:
                    return 'Y';
            }
            break;
        case 'B':
            switch (desired_result) {
                case result::loss:
                    return 'X';
                case result::tie:
                    return 'Y';
                case result::win:
                    return 'Z';
            }
            break;
        case 'C':
            switch (desired_result) {
                case result::loss:
                    return 'Y';
                case result::tie:
                    return 'Z';
                case result::win:
                    return 'X';
            }
            break;
    }
    throw input_error(fmt::format("Unsupported input {} {}", them, r));
}

int score_round(const std::pair<char, char> p)
{
    const auto [l, r]{p};
    return score_result(winner(p)) + score_shape(r);
}

}  // namespace

aoc::solution_result day02(std::string_view input)
{
    const auto strategy_guide{sv_lines(input) | rv::transform([](auto line) {
                                  return std::make_pair(line[0], line[2]);
                              }) |
                              r::to<std::vector>};

    for (const auto& pair : strategy_guide) {
        fmt::print("{} {} {}\n", pair, score_shape(pair.second),
                   score_round(pair));
    }

    const auto part1_score{
        r::accumulate(strategy_guide | rv::transform(score_round), 0)};

    const auto part2_strategy_guide{
        strategy_guide | rv::transform([](const auto& pair) {
            const auto [l, r]{pair};
            return std::make_pair(l, select_response(pair));
        })};

    const auto part2_score{
        r::accumulate(part2_strategy_guide | rv::transform(score_round), 0)};

    return {part1_score, part2_score};
}

}  // namespace aoc::year2022
