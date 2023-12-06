//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <algorithm>
#include <cstdint>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

using range_map = std::map<std::int64_t, std::int64_t>;

range_map flatten_section(const range_map::const_iterator first_section,
                          const range_map& first,
                          const range_map& second)
{
    range_map out;

    auto first_left{first_section};
    auto first_right{std::next(first_left)};
    auto second_left{
        std::prev(second.upper_bound(first_left->first + first_left->second))};
    auto second_right{
        first_right == first.end()
            ? second.end()
            : second.lower_bound(first_right->first + first_left->second)};

    while (second_left != second_right && second_left != second.end() &&
           (first_right == first.end() ||
            (second_left->first - first_left->second < first_right->first))) {
        auto key{std::max(first_left->first,
                          second_left->first - first_left->second)};
        out[key] = first_left->second + second_left->second;
        second_left++;
    }

    return out;
}

range_map flatten_maps(const range_map& first, const range_map& second)
{
    range_map out;
    auto first_iter{first.begin()};
    auto second_iter{first.begin()};

    if (first_iter == first.end()) {
        throw input_error("empty 'first' map");
    }
    if (second_iter == second.end()) {
        throw input_error("empty 'second' map");
    }

    // Merge a range from `first` will all the range sections of `second` it
    // spans
    for (auto iter{first_iter}; iter != first.end(); iter++) {
        out.merge(flatten_section(iter, first, second));
    }

    return out;
}

}  // namespace

aoc::solution_result day05(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    const std::vector<std::int64_t> initial_seeds{
        numbers<std::int64_t>(lines[0].substr(7)) | r::to<std::vector>};

    const auto sections{lines | rv::drop(2) | rv::split("")};
    std::vector<range_map> section_offset_maps;

    for (auto&& section : sections) {
        range_map section_offset_map;
        section_offset_map[0] = 0;
        for (auto line : section | rv::drop(1)) {
            std::array<std::int64_t, 3> nums;
            auto& [dest, src, count]{nums};
            r::copy(numbers<std::int64_t>(line), nums.begin());

            section_offset_map[src] = dest - src;
            section_offset_map[src + count];  // Initialize to 0 if not present
        }
        section_offset_maps.emplace_back(std::move(section_offset_map));
    }

    range_map flattened_map{{{0LL, 0LL}}};
    for (const auto& map : section_offset_maps /* | rv::take(3)*/) {
        flattened_map = flatten_maps(flattened_map, map);
    }

    auto apply_map_func{[](const range_map& map) {
        return [&](std::int64_t key) {
            return key + std::prev(map.upper_bound(key))->second;
        };
    }};

    const std::int64_t part1{
        r::min(initial_seeds | rv::transform(apply_map_func(flattened_map)))};

    auto pairs{initial_seeds | rv::chunk(2) | rv::transform([](auto&& p) {
                   return std::pair<std::int64_t, std::int64_t>{p[0], p[1]};
               })};
    std::map<std::int64_t, bool> seed_range_map{{0, false}};
    for (const auto& [seed, length] : pairs) {
        seed_range_map[seed] = true;
        seed_range_map.try_emplace(seed + length, false);
    }

    std::int64_t part2{9999999999};
    for (const auto& [key, value] : flattened_map) {
        auto prev{key - 1};
        if (prev >= 0) {
            auto seed_bounds{*std::prev(seed_range_map.upper_bound(prev))};
            if (seed_bounds.second) {
                part2 = std::min(part2, apply_map_func(flattened_map)(prev));
            }
        }
        auto seed_bounds{*std::prev(seed_range_map.upper_bound(key))};
        if (seed_bounds.second) {
            part2 = std::min(part2, apply_map_func(flattened_map)(key));
        }
    }

    return {part1, part2};
}

}  // namespace aoc::year2023
