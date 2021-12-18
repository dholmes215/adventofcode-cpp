//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "snailfish.hpp"

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <string_view>

namespace aoc::year2021 {

namespace {

}  // namespace

aoc::solution_result day18(std::string_view input)
{
    std::vector snail_nums{sv_lines(input) | rv::transform(parse_snail) |
                           r::to<std::vector>};

    auto sum{snail_nums[0]};
    for (const auto& num : snail_nums | rv::drop(1)) {
        sum = sum + num;
    }

    regular_num_t largest_magnitude_of_two{0};
    for (const auto& num1 : snail_nums) {
        for (const auto& num2 : snail_nums) {
            if (num1 != num2) {
                largest_magnitude_of_two = std::max(largest_magnitude_of_two,
                                                    (num1 + num2).magnitude());
            }
        }
    }

    return {sum.magnitude(), largest_magnitude_of_two};
}

}  // namespace aoc::year2021
