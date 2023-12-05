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

// range_map flatten_maps(const range_map& first, const range_map& second) {
//     range_map out;
//     auto first_iter{first.begin()};
//     auto second_iter{first.begin()};

//     auto first_to_second{first_iter->second}
// }

}  // namespace

aoc::solution_result day05(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    const std::vector<std::int64_t> initial_seeds{
        numbers<std::int64_t>(lines[0].substr(7)) | r::to<std::vector>};

    const auto sections{lines | rv::drop(2) | rv::split("")};
    std::vector<std::map<std::int64_t, std::int64_t>> section_offset_maps;

    for (auto&& section : sections) {
        std::map<std::int64_t, std::int64_t> section_offset_map;
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

    const auto follow_all_maps{[&](std::int64_t seed) {
        for (const auto& map : section_offset_maps) {
            seed += std::prev(map.upper_bound(seed))->second;
        }
        return seed;
    }};

    const std::int64_t part1{
        r::min(initial_seeds | rv::transform(follow_all_maps))};

    std::int64_t part2{9999999999};
    auto pairs{initial_seeds | rv::chunk(2) | rv::transform([](auto&& p) {
                   return std::pair<std::int64_t, std::int64_t>{p[0], p[1]};
               })};
    for (const auto& [seed, length] : pairs) {
        for (std::int64_t i{0}; i < length; i++) {
            part2 = std::min(part2, follow_all_maps(seed + i));
        }
    }

    return {part1, part2};
}

}  // namespace aoc::year2023
