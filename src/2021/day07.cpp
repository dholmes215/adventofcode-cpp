//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <cmath>
#include <vector>

namespace aoc::year2021 {

namespace {

using crab_int = int;

crab_int distance_to(crab_int src, crab_int dest) noexcept
{
    return std::abs(src - dest);
}

crab_int triangular_number(crab_int n) noexcept
{
    return n * (n + 1) / 2;
}

crab_int fuel_func_a(crab_int src, crab_int dest) noexcept
{
    return distance_to(src, dest);
}

crab_int fuel_func_b(crab_int src, crab_int dest) noexcept
{
    return triangular_number(distance_to(src, dest));
}

crab_int candidate_fuel(auto&& crabs,
                        auto fuel_func,
                        crab_int candidate) noexcept
{
    auto calc_fuel{[=](crab_int pos) { return fuel_func(pos, candidate); }};
    return r::accumulate(crabs | rv::transform(calc_fuel), crab_int{0});
}

crab_int calc_fuel(auto&& crab_positions, auto fuel_func)
{
    const auto [min_pos, max_pos] = r::minmax(crab_positions);
    const crab_int total_distance{max_pos - min_pos + 1};
    const auto candidate_positions{rv::iota(min_pos, total_distance)};

    auto fuel_for_position{[=](crab_int pos) {
        return candidate_fuel(crab_positions, fuel_func, pos);
    }};
    return r::min(candidate_positions | rv::transform(fuel_for_position));
}

}  // namespace

aoc::solution_result day07(std::string_view input)
{
    const auto crab_positions{numbers<crab_int>(input) | r::to<std::vector>};
    return {calc_fuel(crab_positions, fuel_func_a),
            calc_fuel(crab_positions, fuel_func_b)};
}

}  // namespace aoc::year2021
