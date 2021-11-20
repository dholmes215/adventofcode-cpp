//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>

#include <fmt/core.h>

#include <string>

namespace aoc::year2015 {

aoc::solution_result day01(std::istream& input)
{
    int floor{0};
    int position{1};
    std::optional<int> first_position_in_basement{};
    for (const char c : istream_range{input}) {
        if (c == '(') {
            floor++;
        }
        else if (c == ')') {
            floor--;
        }
        else if (!is_whitespace(c)) {
            fmt::print(stderr, "Read unexpected input character '{}'.\n", c);
        }
        if (floor < 0 && !first_position_in_basement) {
            first_position_in_basement = position;
        }
        position++;
    }

    return {std::to_string(floor), std::to_string(*first_position_in_basement)};
}

}  // namespace aoc::year2015