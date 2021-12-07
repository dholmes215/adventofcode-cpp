//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "aoc_solutions.hpp"

#include <year2015.hpp>
#include <year2021.hpp>

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
        {{2015, 3}, {{aoc::year2015::day03, ""}}},
        {{2015, 4}, {{aoc::year2015::day04, ""}}},
        {{2015, 5}, {{aoc::year2015::day05, ""}}},
        {{2015, 6}, {{aoc::year2015::day06, ""}}},
        {{2015, 7}, {{aoc::year2015::day07, ""}}},
        {{2015, 8}, {{aoc::year2015::day08, ""}}},
        {{2021, 1}, {{aoc::year2021::day01, ""}}},
        {{2021, 2}, {{aoc::year2021::day02, ""}}},
        {{2021, 3}, {{aoc::year2021::day03, ""}}},
        {{2021, 4}, {{aoc::year2021::day04, ""}}},
        {{2021, 5},
         {
             {aoc::year2021::day05map, "map"},
             {aoc::year2021::day05hash, "hash"},
             {aoc::year2021::day05sortvec, "sortvec"},
             {aoc::year2021::day05grid, "grid"},
         }},
        {{2021, 6}, {{aoc::year2021::day06, ""}}},
        {{2021, 7}, {{aoc::year2021::day07, ""}}}};
    return solutions;
}

}  // namespace aoc
