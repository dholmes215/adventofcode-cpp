//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc_solutions.hpp>

#include <fmt/core.h>

#include <fstream>

int main()
{
    const auto& solutions{aoc::solutions()};
    for (const auto& [id, solution] : solutions) {
        // Load input
        std::ifstream input;
        // TODO

        // Solve problem
        const auto& result = solution(input);

        fmt::print("{} {} {}\n", id, result.part_a, result.part_b);
    }
}
