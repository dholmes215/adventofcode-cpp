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

namespace {

}  // namespace

aoc::solution_result day01(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    const auto elf_line_groups{lines | rv::split(std::string_view{""}) |
                               rv::transform([](const auto elf_snacks) {
                                   return elf_snacks | r::to<std::vector>;
                               }) |
                               r::to<std::vector>};

    auto elf_calories{elf_line_groups | rv::transform([](const auto group) {
                          return r::accumulate(
                              group | rv::transform([](const auto line) {
                                  return to_int(line);
                              }),
                              0);
                      }) |
                      r::to<std::vector>};

    std::partial_sort(elf_calories.begin(), elf_calories.begin() + 3,
                      elf_calories.end(), std::greater<int>{});

    return {elf_calories.front(), r::accumulate(elf_calories | rv::take(3), 0)};
}

}  // namespace aoc::year2022
