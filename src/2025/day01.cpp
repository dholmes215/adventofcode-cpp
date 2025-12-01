//
// Copyright (c) 2025 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <algorithm>
#include <string_view>
#include <vector>

namespace aoc::year2025 {

namespace {

constexpr std::int64_t dial_start = 50;
constexpr std::int64_t dial_max = 100;

struct rotation {
    char direction;
    int turns;
};

rotation parse_rotation(std::string_view line)
{
    return {line[0], to_int(line.substr(1))};
}

}  // namespace

aoc::solution_result day01(std::string_view input)
{
    input = trim(input);
    const auto rotations =
        sv_lines(input) | rv::transform(parse_rotation) | r::to<std::vector>;

    auto dial = dial_start;
    int p1_zero_count = 0;
    int p2_zero_count = 0;
    for (const auto& r : rotations) {
        if (r.direction == 'L') {
            for (int i = 0; i < r.turns; i++) {
                dial--;
                if (dial == -1) {
                    dial = dial_max - 1;
                }
                if (dial == 0) {
                    p2_zero_count++;
                }
            }
        }
        else {
            for (int i = 0; i < r.turns; i++) {
                dial++;
                if (dial == dial_max) {
                    dial = 0;
                }
                if (dial == 0) {
                    p2_zero_count++;
                }
            }
        }

        if (dial == 0) {
            p1_zero_count++;
        }
        fmt::print("Rotating {} {} times, dial is {}\n",
                   r.direction, r.turns, dial);
    }

    return {p1_zero_count, p2_zero_count};
}

}  // namespace aoc::year2025
