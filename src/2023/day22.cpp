//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
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

namespace aoc::year2023 {

namespace {

using int_t = std::int64_t;

std::array<int_t, 6> parse_numbers(std::string_view line) {
    std::array<int_t, 6> out;
    r::copy(numbers<int_t>(line), out.begin());
    return out;
}

struct numbers_min_max {
    std::array<int_t, 3> min;
    std::array<int_t, 3> max;
};

numbers_min_max find_bounds(const auto& all_numbers) {
    auto first{r::front(all_numbers)};
    numbers_min_max out{{first[0], first[1], first[2]},{first[0], first[1], first[2]}};
    for (const auto& numbers : all_numbers) {
        out.min[0] = std::min(out.min[0], numbers[0]);
        out.min[0] = std::min(out.min[0], numbers[3]);
        out.min[1] = std::min(out.min[1], numbers[1]);
        out.min[1] = std::min(out.min[1], numbers[4]);
        out.min[2] = std::min(out.min[2], numbers[2]);
        out.min[2] = std::min(out.min[2], numbers[5]);
        out.max[0] = std::max(out.max[0], numbers[0]);
        out.max[0] = std::max(out.max[0], numbers[3]);
        out.max[1] = std::max(out.max[1], numbers[1]);
        out.max[1] = std::max(out.max[1], numbers[4]);
        out.max[2] = std::max(out.max[2], numbers[2]);
        out.max[2] = std::max(out.max[2], numbers[5]);
    }
    return out;
}

}  // namespace

aoc::solution_result day22(std::string_view input)
{
    input = trim(input);

    fmt::print("{}\n", sv_lines(input) | rv::transform(parse_numbers));

    auto [min, max]{find_bounds(sv_lines(input) | rv::transform(parse_numbers) ) };
    fmt::print("Bounds: {} -> {}\n", min, max);


    return {"", ""};
}

}  // namespace aoc::year2023
