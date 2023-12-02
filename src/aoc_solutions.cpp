//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "aoc_solutions.hpp"

#include <year2015.hpp>
#include <year2021.hpp>
#include <year2022.hpp>
#include <year2023.hpp>

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
        {{2015, 9}, {{aoc::year2015::day09, ""}}},
        {{2015, 10}, {{aoc::year2015::day10, ""}}},
        {{2015, 11}, {{aoc::year2015::day11, ""}}},
        {{2015, 12}, {{aoc::year2015::day12, ""}}},
        {{2015, 13}, {{aoc::year2015::day13, ""}}},
        {{2015, 14}, {{aoc::year2015::day14, ""}}},
        {{2015, 15}, {{aoc::year2015::day15, ""}}},
        {{2015, 16}, {{aoc::year2015::day16, ""}}},
        {{2015, 17}, {{aoc::year2015::day17, ""}}},
        {{2015, 18}, {{aoc::year2015::day18, ""}}},
        {{2015, 19}, {{aoc::year2015::day19, ""}}},
        {{2015, 20}, {{aoc::year2015::day20, ""}}},
        {{2015, 21}, {{aoc::year2015::day21, ""}}},
        {{2015, 22}, {{aoc::year2015::day22, ""}}},
        {{2015, 23}, {{aoc::year2015::day23, ""}}},
        {{2015, 24}, {{aoc::year2015::day24, ""}}},
        {{2015, 25}, {{aoc::year2015::day25, ""}}},
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
        {{2021, 7},
         {{aoc::year2021::day07naive, "naive"},
          {aoc::year2021::day07fast, "fast"}}},
        {{2021, 8}, {{aoc::year2021::day08, ""}}},
        {{2021, 9}, {{aoc::year2021::day09, ""}}},
        {{2021, 10}, {{aoc::year2021::day10, ""}}},
        {{2021, 11}, {{aoc::year2021::day11, ""}}},
        {{2021, 12}, {{aoc::year2021::day12, ""}}},
        {{2021, 13}, {{aoc::year2021::day13, ""}}},
        {{2021, 14}, {{aoc::year2021::day14, ""}}},
        {{2021, 15}, {{aoc::year2021::day15, ""}}},
        {{2021, 16}, {{aoc::year2021::day16, ""}}},
        {{2021, 17}, {{aoc::year2021::day17, ""}}},
        {{2021, 18}, {{aoc::year2021::day18, ""}}},
        {{2021, 19}, {{aoc::year2021::day19, ""}}},
        {{2021, 20}, {{aoc::year2021::day20, ""}}},
        {{2021, 21}, {{aoc::year2021::day21, ""}}},
        {{2021, 22}, {{aoc::year2021::day22, ""}}},
        {{2021, 23}, {{aoc::year2021::day23, ""}}},
        {{2021, 24}, {{aoc::year2021::day24, ""}}},
        {{2021, 25}, {{aoc::year2021::day25, ""}}},
        {{2022, 01}, {{aoc::year2022::day01, ""}}},
        {{2022, 02},
         {{aoc::year2022::day02naive, "naive"},
          {aoc::year2022::day02lookup, "lookup"}}},
        {{2022, 3}, {{aoc::year2022::day03, ""}}},
        {{2022, 4}, {{aoc::year2022::day04, ""}}},
        {{2022, 5}, {{aoc::year2022::day05, ""}}},
        {{2022, 6}, {{aoc::year2022::day06, ""}}},
        {{2022, 7}, {{aoc::year2022::day07, ""}}},
        {{2022, 8}, {{aoc::year2022::day08, ""}}},
        {{2022, 9}, {{aoc::year2022::day09, ""}}},
        {{2022, 10}, {{aoc::year2022::day10, ""}}},
        {{2022, 11}, {{aoc::year2022::day11, ""}}},
        {{2022, 12}, {{aoc::year2022::day12, ""}}},
        {{2022, 13}, {{aoc::year2022::day13, ""}}},
        {{2022, 14}, {{aoc::year2022::day14, ""}}},
        {{2022, 15}, {{aoc::year2022::day15, ""}}},
        {{2022, 16}, {{aoc::year2022::day16, ""}}},
        {{2022, 17}, {{aoc::year2022::day17, ""}}},
        {{2022, 18}, {{aoc::year2022::day18, ""}}},
        {{2022, 19}, {{aoc::year2022::day19, ""}}},
        {{2022, 20}, {{aoc::year2022::day20, ""}}},
        {{2022, 21}, {{aoc::year2022::day21, ""}}},
        {{2022, 22}, {{aoc::year2022::day22, ""}}},
        {{2022, 23}, {{aoc::year2022::day23, ""}}},
        {{2022, 24}, {{aoc::year2022::day24, ""}}},
        {{2022, 25}, {{aoc::year2022::day25, ""}}},
        {{2023, 01},
         {{aoc::year2023::day01, ""},
          {aoc::year2023::day01par_unseq, "par_unseq"}}},
        {{2023, 02}, {{aoc::year2023::day02, ""}}},
    };
    return solutions;
}

}  // namespace aoc
