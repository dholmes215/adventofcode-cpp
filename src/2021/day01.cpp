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
    std::vector<int> depths;
    depths.reserve(2048);
    r::copy(int_lines(input), r::back_inserter(depths));
    return depths;
}

auto count_depth_increases(const std::vector<int>& v, int window_size) noexcept
{
    // Part B explanation: the problem asks us to compare the sums of the
    // elements of overlapping windows of size 3:
    //   (a + b + c) < (b + c + d)
    // Subtract (b + c) from both sides and this can be simplified to:
    //   a < d
    // Thus instead of comparing two windows of size 3, we can compare the first
    // and last elements of one window of size 4.  Part A involved comparing the
    // first and last elements of one window of size 2, so the two parts are
    // really the same general problem with different window sizes.

    const auto windows{v | rv::sliding(window_size)};
    using window_t = decltype(windows[0]);
    auto depth_increased{[=](window_t w) { return w[window_size - 1] > w[0]; }};
    return r::count_if(windows, depth_increased);
}

}  // namespace

aoc::solution_result day01(std::string_view input)
{
    const std::vector<int> depths{parse_input(input)};
    const auto part_a{count_depth_increases(depths, 2)};
    const auto part_b{count_depth_increases(depths, 4)};
    return {part_a, part_b};
}

}  // namespace aoc::year2021
