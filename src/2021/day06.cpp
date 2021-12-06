//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <vector>

namespace aoc::year2021 {

namespace {

using timer = int;
using count_type = std::int64_t;
constexpr timer cycle_length{7};
constexpr timer initial_length{9};

std::vector<count_type> populate_initial_counts(
    const std::vector<timer>& fish) noexcept
{
    std::vector<count_type> counts_by_timer(initial_length, 0);
    for (timer number : fish) {
        counts_by_timer[static_cast<std::size_t>(number)]++;
    }
    return counts_by_timer;
}

void run_days(std::vector<count_type>& counts, int days) noexcept
{
    for (int i{0}; i < days; i++) {
        r::rotate(counts, counts.begin() + 1);
        counts[cycle_length - 1] += counts[initial_length - 1];
    }
};

}  // namespace

aoc::solution_result day06(std::string_view input)
{
    const auto fish{numbers<timer>(input) | r::to<std::vector>};
    std::vector<count_type> counts_by_timer(populate_initial_counts(fish));

    run_days(counts_by_timer, 80);
    const auto sum_a{r::accumulate(counts_by_timer, count_type{0})};

    run_days(counts_by_timer, 176);
    const auto sum_b{r::accumulate(counts_by_timer, count_type{0})};

    return {sum_a, sum_b};
}

}  // namespace aoc::year2021
