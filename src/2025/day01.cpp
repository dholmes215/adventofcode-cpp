//
// Copyright (c) 2025 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <string_view>

namespace aoc::year2025 {

namespace {

// To avoid having to deal with wrapping when underflowing, just use a big
// number and only look at the last two digits to know the actual dial value.
constexpr std::int64_t dial_start = 1000000050;
constexpr std::int64_t dial_max = 100;

struct rotation {
    char direction;
    int turns;
};

rotation parse_rotation(std::string_view line)
{
    return {line[0], to_int(line.substr(1))};
}

struct rotate_result {
    std::int64_t dial;
    std::uint64_t zero_count;
};

rotate_result rotate_dial(std::int64_t dial, const rotation r)
{
    uint64_t zero_count = 0;
    const std::int64_t sign = r.direction == 'L' ? -1 : 1;
    const auto new_dial = dial + (sign * r.turns);
    const std::int64_t adjustment =
        r.direction == 'L' ? static_cast<std::int64_t>(new_dial % 100 == 0) -
                                 static_cast<std::int64_t>(dial % 100 == 0)
                           : 0;
    zero_count +=
        std::abs((new_dial / dial_max) - (dial / dial_max)) + adjustment;

    return {new_dial, zero_count};
}

}  // namespace

aoc::solution_result day01(std::string_view input)
{
    input = trim(input);
    const auto rotations = sv_lines(input) | rv::transform(parse_rotation);

    auto dial = dial_start;
    std::uint64_t p1_zero_count = 0;
    std::uint64_t p2_zero_count = 0;
    for (const auto& r : rotations) {
        const auto [new_dial, zero_count] = rotate_dial(dial, r);
        dial = new_dial;
        p2_zero_count += zero_count;
        if (dial % dial_max == 0) {
            p1_zero_count++;
        }
    }

    return {p1_zero_count, p2_zero_count};
}

}  // namespace aoc::year2025
