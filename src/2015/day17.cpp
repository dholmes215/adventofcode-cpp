//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <string_view>

namespace aoc::year2015 {

constexpr int refrigerator_capacity{150};

aoc::solution_result day17(std::string_view input)
{
    auto containers{int_lines(trim(input)) | r::to<std::vector>};
    r::sort(containers);
    const auto all_20bit_ints{rv::iota(0, 0b100000000000000000000)};

    const auto int_to_filtered_containers{[&](int i) {
        auto mask_range{bit_range<bool>(i, 20)};
        return rv::zip(mask_range, containers) |
               rv::filter([](auto pair) { return std::get<0>(pair); }) |
               rv::transform([](auto pair) { return std::get<1>(pair); });
    }};

    const auto sum_subset{[](auto subset) { return r::accumulate(subset, 0); }};
    const auto container_subsets{all_20bit_ints |
                                 rv::transform(int_to_filtered_containers)};
    const auto subset_volumes{container_subsets | rv::transform(sum_subset)};
    const auto matching_subsets{
        container_subsets | rv::filter([&](auto subset) {
            return sum_subset(subset) == refrigerator_capacity;
        }) |
        r::to<std::vector>};

    const auto part1_count{r::distance(matching_subsets)};

    const auto subset_sizes{matching_subsets | rv::transform([](auto subset) {
                                return r::distance(subset);
                            })};
    const auto min_matching_size{r::min(subset_sizes)};
    const auto part2_count{r::count(subset_sizes, min_matching_size)};

    return {part1_count, part2_count};
}

}  // namespace aoc::year2015
