//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <tiny_vector.hpp>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace aoc::year2022 {

namespace {

struct move_t {
    std::size_t count;
    std::size_t source;  // Modified to be indexed from 0, not 1
    std::size_t dest;    // Ditto
};

move_t parse_move(std::string_view line)
{
    const auto parts{sv_words(line) | rv::drop(1) | rv::stride(2) |
                     rv::transform(to_num<std::uint8_t>) |
                     r::to<tiny_vector<std::uint8_t>>};
    return {parts[0], parts[1] - 1ULL, parts[2] - 1ULL};
}

std::vector<std::vector<char>> parse_stacks(
    const std::vector<std::string_view>& stack_lines)
{
    const std::size_t stack_count{
        static_cast<std::size_t>(r::distance(sv_words(stack_lines.back())))};

    const auto stack_from_lines{[&](auto stack_id) {
        auto col{4ULL * stack_id + 1ULL};
        return stack_lines | rv::reverse | rv::drop(1) |
               rv::transform([&](auto line) { return line[col]; }) |
               rv::take_while([](char c) { return c != ' '; }) |
               r::to<std::vector>;
    }};

    return rv::ints(std::size_t{0}, stack_count) |
           rv::transform(stack_from_lines) | r::to<std::vector>;
}

using parsed_input_t =
    std::pair<std::vector<std::vector<char>>, std::vector<move_t>>;
parsed_input_t parse_input(std::string_view input)
{
    auto line_sections{sv_lines(input) | rv::split("") |
                       rv::transform([](const auto& rng) {
                           return rng | r::to<std::vector>;
                       }) |
                       r::to<std::vector>};
    const auto& stack_lines{line_sections[0]};
    const auto& move_lines{line_sections[1]};

    return {parse_stacks(stack_lines),
            move_lines | rv::transform(parse_move) | r::to<std::vector>};
}

}  // namespace

aoc::solution_result day05(std::string_view input)
{
    const auto [initial_stacks, moves]{parse_input(input)};

    auto stacks1{initial_stacks};
    for (const auto& move : moves) {
        auto& from{stacks1[move.source]};
        auto& to{stacks1[move.dest]};
        r::copy(from | rv::reverse | rv::take(move.count),
                std::back_inserter(to));
        from.resize(from.size() - move.count);
    }

    auto stacks2{initial_stacks};
    for (const auto& move : moves) {
        auto& from{stacks2[move.source]};
        auto& to{stacks2[move.dest]};
        r::copy(from | rv::reverse | rv::take(move.count) | rv::reverse,
                std::back_inserter(to));
        from.resize(from.size() - move.count);
    }

    const auto tops{[](const auto& stacks) {
        return stacks |
               rv::transform([](const auto& stack) { return stack.back(); }) |
               r::to<std::string>;
    }};

    return {tops(stacks1), tops(stacks2)};
}

}  // namespace aoc::year2022
