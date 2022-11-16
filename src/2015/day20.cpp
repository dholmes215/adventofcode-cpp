//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <cstdint>
#include <string_view>
#include <vector>

namespace aoc::year2015 {

namespace {

using int_t = std::uint64_t;

}  // namespace

aoc::solution_result day20(std::string_view input)
{
    const auto input_num{to_num<int_t>(trim(input))};
    const auto max_house{input_num / 20};
    std::vector<int_t> present_counts;
    present_counts.resize(max_house + 1);

    const auto check_house{[=](int_t count) { return count >= input_num; }};
    const auto find_house{
        [&] { return r::find_if(present_counts, check_house); }};
    const auto find_house_num{
        [&] { return r::distance(present_counts.begin(), find_house()); }};

    // Naive simulation
    for (int_t elf{1}; elf <= max_house; elf++) {
        for (int_t house{elf}; house <= max_house; house += elf) {
            present_counts[house] += elf * 10;
        }
    }
    const auto part1_house{find_house_num()};

    present_counts.clear();
    present_counts.resize(max_house + 1);

    // Naive simulation
    for (int_t elf{1}; elf <= max_house; elf++) {
        int_t visit{0};
        for (int_t house{elf}; house <= max_house && visit < 50;
             house += elf, visit++) {
            present_counts[house] += elf * 11;
        }
    }
    const auto part2_house{find_house_num()};

    return {part1_house, part2_house};
}

}  // namespace aoc::year2015
