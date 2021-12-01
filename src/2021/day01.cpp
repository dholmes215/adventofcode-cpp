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

std::vector<int> parse_input(std::string_view input)
{
    // return int_lines(input) | r::to<std::vector>();

    std::vector<int> depths;
    depths.reserve(2048);
    r::copy(int_lines(input), r::back_inserter(depths));
    return depths;
}

bool back_greater_than_front(auto&& rng) noexcept
{
    return *r::prev(rng.end()) > r::front(rng);
}

}  // namespace

aoc::solution_result day01(std::string_view input)
{
    const std::vector<int> depths{parse_input(input)};

    const auto part_a{r::count_if(depths | rv::sliding(2), [](auto&& rng) {
        return back_greater_than_front(rng);
    })};

    const auto part_b{r::count_if(depths | rv::sliding(4), [](auto&& rng) {
        return back_greater_than_front(rng);
    })};

    return {part_a, part_b};
}

}  // namespace aoc::year2021
