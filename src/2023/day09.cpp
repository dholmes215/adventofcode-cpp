//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <algorithm>
#include <numeric>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

using value = std::int32_t;

std::vector<value> parse_sequence(std::string_view line)
{
    return numbers<value>(line) | r::to<std::vector>;
}

std::vector<std::vector<value>> generate_adjacent_diffs(
    const std::vector<value>& sequence)
{
    std::vector<std::vector<value>> out;
    out.push_back(sequence);
    const auto is_zero{[](value v) { return v == 0; }};
    while (!r::all_of(out.back(), is_zero)) {
        out.emplace_back();
        const auto& prev_back{out[out.size() - 2]};
        auto& new_back{out.back()};
        new_back = prev_back | rv::sliding(2) |
                   rv::transform([](auto&& rng) { return rng[1] - rng[0]; }) |
                   r::to<std::vector>;
    }
    return out;
}

}  // namespace

aoc::solution_result day09(std::string_view input)
{
    std::vector<std::vector<value>> input_sequences{
        sv_lines(trim(input)) | rv::transform(parse_sequence) |
        r::to<std::vector>};

    std::vector<std::vector<std::vector<value>>> all_adjacent_diffs{
        input_sequences | rv::transform(generate_adjacent_diffs) |
        r::to<std::vector>};

    const auto part1{r::accumulate(
        all_adjacent_diffs | rv::join | rv::transform(r::back), 0)};

    for (auto& adjacent_diffs : all_adjacent_diffs) {
        for (std::vector<value>& sequence : adjacent_diffs) {
            r::reverse(sequence);
        }
    }

    value part2{0};
    for (auto& adjacent_diffs : all_adjacent_diffs) {
        adjacent_diffs.back().push_back(0);
        for (std::size_t i{0}; i < adjacent_diffs.size() - 1; i++) {
            auto& bottom{adjacent_diffs[adjacent_diffs.size() - 1 - i]};
            auto& top{adjacent_diffs[adjacent_diffs.size() - 2 - i]};
            top.push_back(top.back() - bottom.back());
        }

        part2 += adjacent_diffs.front().back();
    }

    return {part1, part2};
}

}  // namespace aoc::year2023
