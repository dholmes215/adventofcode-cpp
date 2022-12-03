//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <bit>
#include <cstdint>
#include <functional>
#include <string_view>

namespace aoc::year2022 {

namespace {

constexpr int priority(char c)
{
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 1;
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 27;
    }
    throw input_error(fmt::format("Invalid item {}", c));
}

constexpr std::uint64_t priority_mask(std::string_view items)
{
    std::uint64_t out{0};
    for (char c : items) {
        out |= 1ULL << priority(c);
    }
    return out;
}

constexpr std::uint64_t common_items(std::string_view rucksack)
{
    const std::string_view c1{rucksack.substr(0, rucksack.size() / 2)};
    const std::string_view c2{rucksack.substr(rucksack.size() / 2)};
    return priority_mask(c1) & priority_mask(c2);
}

constexpr auto common_items_group{[](const auto& mask_range) {
    return r::accumulate(mask_range, ~std::uint64_t{0}, std::bit_and{});
}};

constexpr int priority_from_mask(std::uint64_t mask)
{
    return std::bit_width(mask) - 1;
}

constexpr int priority_from_rucksack(std::string_view rucksack)
{
    return priority_from_mask(common_items(rucksack));
}

}  // namespace

aoc::solution_result day03(std::string_view input)
{
    const auto rucksacks{sv_lines(trim(input)) | r::to<std::vector>};

    const auto part1_sum{
        r::accumulate(rucksacks | rv::transform(priority_from_rucksack), 0)};

    const auto part2_sum{
        r::accumulate(rucksacks | rv::transform(priority_mask) | rv::chunk(3) |
                          rv::transform(common_items_group) |
                          rv::transform(priority_from_mask),
                      0)};

    return {part1_sum, part2_sum};
}

}  // namespace aoc::year2022
