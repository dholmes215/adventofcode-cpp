//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <charconv>

namespace aoc::year2021 {

namespace {

int to_int(std::string_view sv)
{
    int out{0};
    const auto result{std::from_chars(&*sv.begin(), &*sv.end(), out)};
    if (result.ec != std::errc{}) {
        throw input_error{
            fmt::format("error parsing \"{}\" as int: {}", sv, result.ec)};
    }
    return out;
}

bool back_greater_than_front(auto&& rng) noexcept
{
    return *r::prev(rng.end()) > r::front(rng);
}

}  // namespace

aoc::solution_result day01(std::string_view input)
{
    const auto depths{sv_lines(input) | rv::transform(to_int) |
                      r::to<std::vector>()};

    const auto part_a{r::count_if(depths | rv::sliding(2), [](auto&& rng) {
        return back_greater_than_front(rng);
    })};

    const auto part_b{r::count_if(depths | rv::sliding(4), [](auto&& rng) {
        return back_greater_than_front(rng);
    })};

    return {part_a, part_b};
}

}  // namespace aoc::year2021
