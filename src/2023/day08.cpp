//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <algorithm>
#include <map>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

// We convert 'L' and 'R' to 0 and 1 to use them as array indexes
int instruction_as_index(char instruction)
{
    switch (instruction) {
        case 'L':
            return 0;
        case 'R':
            return 1;
    }
    throw input_error(fmt::format("Invalid instruction: {}\n", instruction));
}

using map_t = std::unordered_map<std::string_view, std::array<std::string_view, 2>>;

map_t::value_type parse_node(std::string_view line)
{
    return {line.substr(0, 3), {line.substr(7, 3), line.substr(12, 3)}};
}

std::int64_t gcd(std::int64_t a, std::int64_t b) {
    while (b != 0) {
        std::int64_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

std::int64_t lcm(std::int64_t a, std::int64_t b) {
    return (a * b) / gcd(a, b);
}

std::int64_t lcm_vector(const std::vector<int>& numbers) {
    std::int64_t result = numbers[0];
    for (size_t i = 1; i < numbers.size(); ++i) {
        result = lcm(result, numbers[i]);
    }

    return result;
}

}  // namespace

aoc::solution_result day08(std::string_view input)
{
    input = trim(input);
    const auto lines{sv_lines(input) | r::to<std::vector>};
    const std::string_view instructions{r::front(lines)};
    const auto cycled_idx_instructions{
        instructions | rv::transform(instruction_as_index) | rv::cycle};

    const map_t nodes{lines | rv::drop(2) | rv::transform(parse_node) |
                      r::to<map_t>};

    const auto solve_part1{[&](std::string_view start_node,
                               auto&& is_end_predicate) {
        int part1_steps_taken{0};
        std::string_view part1_current_node{start_node};

        auto part1_current_instruction_iter{r::begin(cycled_idx_instructions)};
        while (!is_end_predicate(part1_current_node)) {
            part1_steps_taken++;
            int inst{*part1_current_instruction_iter++};
            part1_current_node = nodes.at(part1_current_node)[inst];
        }
        return part1_steps_taken;
    }};

    int part1{solve_part1("AAA",
                          [](std::string_view node) { return node == "ZZZ";
                          })};

    const auto is_start_node{
        [](std::string_view node) { return node[2] == 'A'; }};
    const auto is_end_node{
        [](std::string_view node) { return node[2] == 'Z'; }};
    std::vector<std::string_view> part2_start_nodes{
        nodes | rv::keys | rv::filter(is_start_node) | r::to<std::vector>};
    const auto part2_transform_func{
        [&](std::string_view key) { return solve_part1(key, is_end_node); }};
    const std::vector<int> part2_steps_taken{
        part2_start_nodes | rv::transform(part2_transform_func) |
        r::to<std::vector>};

    return {part1, lcm_vector(part2_steps_taken)};
}

}  // namespace aoc::year2023
