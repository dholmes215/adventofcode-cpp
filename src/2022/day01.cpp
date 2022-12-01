//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <algorithm>
#include <string_view>
#include <vector>

namespace aoc::year2022 {

aoc::solution_result day01(std::string_view input)
{
    const auto snack_lines_to_ints{[](const auto elf_snacks) {
        return elf_snacks | rv::transform(to_int);
    }};

    const auto elf_snacks{sv_lines(trim(input)) | rv::split("") |
                          rv::transform(snack_lines_to_ints)};

    const auto add_snacks{
        [](const auto& snack_rng) { return r::accumulate(snack_rng, 0); }};

    auto elf_calories{elf_snacks | rv::transform(add_snacks) |
                      r::to<std::vector>};

    r::partial_sort(elf_calories, elf_calories.begin() + 3, std::greater{});

    return {elf_calories.front(), r::accumulate(elf_calories | rv::take(3), 0)};
}

}  // namespace aoc::year2022
