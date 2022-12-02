//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <string_view>
#include <utility>

namespace aoc::year2022 {

namespace {

constexpr int score_shape(const char c)
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

constexpr result winner(const std::pair<char, char> p)
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

constexpr int score_result(const result r)
{
    switch (r) {
        case result::win:
            return 6;
        case result::tie:
            return 3;
        case result::loss:
            return 0;
    }
    throw input_error(
        fmt::format("Unsupported result: {}", static_cast<int>(r)));
}

constexpr result needed_result(const char c)
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

constexpr char select_response(const std::pair<char, char> p)
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

constexpr int score_round(const std::pair<char, char> p)
{
    const auto [l, r]{p};
    return score_result(winner(p)) + score_shape(r);
}

constexpr std::pair<char, char> int_to_pair(int i)
{
    return {static_cast<char>(i % 3 + 'A'), static_cast<char>(i / 3 + 'X')};
}

constexpr std::array<int, 9> part1_lookup_func()
{
    std::array<int, 9> out;
    for (int i{0}; i < 9; i++) {
        out[static_cast<std::size_t>(i)] = score_round(int_to_pair(i));
    }
    return out;
}

constinit const auto part1_lookup_table{part1_lookup_func()};

constexpr int int_to_part1_score(int i)
{
    return part1_lookup_table[static_cast<std::size_t>(i)];
}

constexpr std::array<int, 9> part2_lookup_func()
{
    std::array<int, 9> out;
    for (int i{0}; i < 9; i++) {
        const auto pair{int_to_pair(i)};
        const auto [l, r]{pair};
        out[static_cast<std::size_t>(i)] =
            score_round({l, select_response(pair)});
    }
    return out;
}

constinit const auto part2_lookup_table{part2_lookup_func()};

constexpr int int_to_part2_score(int i)
{
    return part2_lookup_table[static_cast<std::size_t>(i)];
}

constexpr std::pair<int, int> int_to_both_scores(int i)
{
    return {int_to_part1_score(i), int_to_part2_score(i)};
}

constexpr int chars_to_int(char l, char r)
{
    return l - 'A' + (r - 'X') * 3;
}

}  // namespace

aoc::solution_result day02naive(std::string_view input)
{
    const auto strategy_guide{sv_lines(input) | rv::transform([](auto line) {
                                  return std::make_pair(line[0], line[2]);
                              }) |
                              r::to<std::vector>};

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

aoc::solution_result day02lookup(std::string_view input)
{
    const auto chunk_to_int{
        [](auto chunk) { return chars_to_int(chunk[0], chunk[2]); }};
    const auto strategy_guide{input | rv::chunk(4) |
                              rv::transform(chunk_to_int)};

    const auto [part1_score, part2_score]{r::accumulate(
        strategy_guide | rv::transform(int_to_both_scores),
        std::pair<int, int>{0, 0}, [](const auto l, const auto r) {
            return std::pair<int, int>{l.first + r.first, l.second + r.second};
        })};

    return {part1_score, part2_score};
}

}  // namespace aoc::year2022
