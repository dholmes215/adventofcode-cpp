//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

namespace aoc::year2021 {

namespace {

}  // namespace

using timer = int;
using count_type = std::int64_t;
constexpr timer cycle_length{7};
constexpr timer initial_length{9};

aoc::solution_result day06(std::string_view input)
{
    auto fish{numbers<timer>(input)};
    const auto vec{fish | r::to<std::vector>};

    std::vector<count_type> counts_by_timer(initial_length, 0);
    for (timer number : vec) {
        counts_by_timer[static_cast<std::size_t>(number)]++;
    }

    auto run_days{[&](int days){
        for (int i = 0; i < days; i++) {
            r::rotate(counts_by_timer, counts_by_timer.begin() + 1);
            counts_by_timer[cycle_length - 1] +=
                counts_by_timer[initial_length - 1];

        }
    }};

    run_days(80);
    const auto sum_a{r::accumulate(counts_by_timer, count_type{0})};

    run_days(176);
    const auto sum_b{r::accumulate(counts_by_timer, count_type{0})};

    return {sum_a, sum_b};
}

}  // namespace aoc::year2021
