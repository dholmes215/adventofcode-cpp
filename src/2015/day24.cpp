//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <coro_generator.hpp>

#include <fmt/ranges.h>

#include <string_view>
#include <vector>

namespace aoc::year2015 {

namespace {

// TODO: Return range instead of vector?  Take range input instead of vector?
template <typename T>
Generator<std::vector<T>> powerset(const std::vector<T>& set)
{
    const auto input_size{set.size()};

    // XXX This should logically be a `std::vector<bool>`
    std::vector<std::uint8_t> membership_bits;
    membership_bits.resize(input_size);

    for (std::size_t subset_size{0}; subset_size < input_size; subset_size++) {
        membership_bits[subset_size] = 1;
        do {
            co_yield set | rv::enumerate | rv::filter([&](const auto& pair) {
                const auto& [index, _]{pair};
                return membership_bits[index];
            }) | rv::values |
                r::to<std::vector>;
        } while (std::prev_permutation(membership_bits.begin(),
                                       membership_bits.end()));
    }
}

int range_sum(const auto& rng)
{
    return r::accumulate(rng, 0);
}

std::uint64_t vector_product(const std::vector<int>& v)
{
    return r::accumulate(v, 1ULL, std::multiplies<std::uint64_t>{});
}

std::uint64_t solve_for_group_count(const std::vector<int> packages,
                                    int group_count)
{
    const auto total_weight{r::accumulate(packages, 0)};
    const auto group_weight{total_weight / group_count};

    const auto filter_subsets{[group_weight](const auto& rng) {
        return range_sum(rng) == group_weight;
    }};

    std::optional<std::ptrdiff_t> target_size;
    const auto filter_target_size{[&target_size](const auto& rng) {
        const auto size{r::distance(rng)};
        if (!target_size) {
            target_size = size;
            return true;
        }
        return size == *target_size;
    }};

    auto subsets{powerset(packages)};
    const auto matching_subsets{subsets | rv::filter(filter_subsets) |
                                rv::take_while(filter_target_size) |
                                r::to<std::vector>};
    return r::min(matching_subsets | rv::transform(vector_product));
}

}  // namespace

aoc::solution_result day24(std::string_view input)
{
    const std::vector<int> packages{int_lines(trim(input)) |
                                    r::to<std::vector>};
    return {solve_for_group_count(packages, 3),
            solve_for_group_count(packages, 4)};
}

}  // namespace aoc::year2015
