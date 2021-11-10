//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "aoc_solutions.hpp"

#include <year2015.hpp>

#include <fmt/core.h>

namespace aoc {

const std::map<solution_id, solution_func>& solutions()
{
    static const std::map<solution_id, solution_func> solutions{
        {{2015, 1}, aoc::year2015::day01}};
    return solutions;
}

}  // namespace aoc
