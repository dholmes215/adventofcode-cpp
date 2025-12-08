//
// Copyright (c) 2025 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <string_view>
#include <vector>

namespace aoc::year2025 {

aoc::solution_result day07(std::string_view input)
{
    const auto input_lines = sv_lines(trim(input)) | r::to<std::vector>;
    std::vector<std::vector<std::uint64_t>> beam_timeline_counts(input_lines.size());
    r::fill(beam_timeline_counts,
            std::vector<std::uint64_t>(input_lines[0].size(), 0));

    std::uint64_t split_count = 0;

    beam_timeline_counts[0][input_lines[0].find('S')] = 1;

    for (const auto [i1, i2] : rv::iota(0UZ, beam_timeline_counts.size()) |
                                   rv::sliding(2) |
                                   rv::transform([](auto&& rng) {
                                       return std::pair{rng[0], rng[1]};
                                   })) {
        const auto& count_line = beam_timeline_counts[i1];
        auto& count_next_line = beam_timeline_counts[i2];
        const auto& input_next_line = input_lines[i2];

        for (auto [i, count] : count_line | rv::enumerate) {
            if (input_next_line[i] == '^') {
                count_next_line[i-1] += count;
                count_next_line[i+1] += count;
                if (count > 0) {
                    split_count++;
                }
            } else {
                count_next_line[i] += count;
            }
        }
    }

    const auto a = split_count;
    const auto b = r::accumulate(beam_timeline_counts.back(), 0UZ);

    return {a, b};
}

}  // namespace aoc::year2025
