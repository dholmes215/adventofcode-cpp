//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "aoc_solutions.hpp"

#include <year2015.hpp>

#include <fmt/core.h>

#include <vector>

namespace aoc {

const std::map<date, std::vector<solution>>& solutions()
{
    static const std::map<date, std::vector<solution>> solutions{
        {{2015, 1},
         {{aoc::year2015::day01, "imperative"},
          {aoc::year2015::day01algorithm, "algorithm"},
          {aoc::year2015::day01ranges, "ranges"}}},
        {{2015, 2}, {{aoc::year2015::day02, ""}}},
        {{2015, 3}, {{aoc::year2015::day03, ""}}}};
    return solutions;
}

}  // namespace aoc
