//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_SOLUTIONS_HPP
#define AOC_SOLUTIONS_HPP

#include <aoc.hpp>

#include <fmt/core.h>
#include <fmt/format.h>

#include <map>
#include <vector>

namespace aoc {

const std::map<date, std::vector<solution>>& solutions();

}  // namespace aoc

#endif  // AOC_SOLUTIONS_HPP
