//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <cstdlib>
#include <numeric>

namespace aoc::year2021 {

using depth = int;

depth to_int(std::string_view sv)
{
    return std::atoi(sv.data());
}

int is_positive(depth d)
{
    return d > 0 ? 1 : 0;
}

depth difference(auto&& rng)
{
    return *(++rng.begin()) - *rng.begin();
}

aoc::solution_result day01(std::string_view input)
{
    const auto depths{sv_lines(input) | rv::transform(to_int) |
                      r::to<std::vector>()};

    auto part_a{r::count_if(
        depths | rv::sliding(2) |
            rv::transform([](auto&& rng) { return difference(rng); }),
        is_positive)};

    auto part_b{r::count_if(
        depths | rv::sliding(3) | rv::transform([](auto&& rng) {
            return r::accumulate(rng, 0);
        }) | rv::sliding(2) |
            rv::transform([](auto&& rng) { return difference(rng); }),
        is_positive)};

    return {part_a, part_b};
}

}  // namespace aoc::year2021
