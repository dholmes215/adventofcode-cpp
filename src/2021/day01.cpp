//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <charconv>

namespace aoc::year2021 {

int to_int(std::string_view sv)
{
    int out{0};
    std::from_chars(&*sv.begin(), &*sv.end(), out);
    return out;
}

bool back_greater_than_front(auto&& rng)
{
    return *r::prev(rng.end()) > r::front(rng);
}

aoc::solution_result day01(std::string_view input)
{
    const auto depths{sv_lines(input) | rv::transform(to_int) |
                      r::to<std::vector>()};

    auto part_a{r::count_if(depths | rv::sliding(2), [](auto&& rng) {
        return back_greater_than_front(rng);
    })};

    auto part_b{r::count_if(depths | rv::sliding(4), [](auto&& rng) {
        return back_greater_than_front(rng);
    })};

    return {part_a, part_b};
}

}  // namespace aoc::year2021
