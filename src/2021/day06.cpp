//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <cstdint>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

namespace {

using timer_int = int;
using count_type = std::int64_t;
constexpr timer_int cycle_length{7};
constexpr timer_int initial_length{9};
constexpr int part1_days{80};
constexpr int part2_days{256};

std::vector<count_type> populate_initial_counts(
    const std::vector<timer_int>& fish) noexcept
{
    std::vector<count_type> counts_by_timer(initial_length, 0);
    for (timer_int number : fish) {
        counts_by_timer[static_cast<std::size_t>(number)]++;
    }
    return counts_by_timer;
}

void run_day(std::vector<count_type>& counts) noexcept
{
    r::rotate(counts, counts.begin() + 1);
    // The number we rotated into the last position will represent the newborns;
    // the parent fish restart their cycle at position 6.
    counts[cycle_length - 1] += counts[initial_length - 1];
}

void run_days(std::vector<count_type>& counts, int days) noexcept
{
    for (int i{0}; i < days; i++) {
        run_day(counts);
    }
}

}  // namespace

aoc::solution_result day06(std::string_view input)
{
    const auto fish{numbers<timer_int>(input) | r::to<std::vector>};
    std::vector<count_type> counts_by_timer(populate_initial_counts(fish));

    run_days(counts_by_timer, part1_days);
    const auto sum_a{r::accumulate(counts_by_timer, count_type{0})};

    run_days(counts_by_timer, part2_days - part1_days);
    const auto sum_b{r::accumulate(counts_by_timer, count_type{0})};

    return {sum_a, sum_b};
}

}  // namespace aoc::year2021
