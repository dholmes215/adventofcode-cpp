//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <string_view>
#include <vector>

namespace aoc::year2021 {

int binary_range_to_int(auto&& r)
{
    int result{0};
    for (bool b : r) {
        const int i{b ? 1 : 0};
        result = (result << 1) | i;
    }
    return result;
}

int binary_to_int(std::string_view s)
{
    return to_int_base(s, 2);
}

auto bit_range(int i, std::size_t count)
{
    auto generator{[=]() mutable { return (i >> --count) & 1; }};
    return rv::generate_n(generator, count);
}

std::vector<int> add_rows(std::vector<int>& row1,
                          decltype(bit_range(0, 12)) row2)
{
    r::copy(rv::zip_with(std::plus<int>(), row1, row2), row1.begin());
    return std::vector(std::move(row1));
}

// std::vector<int> count_bits(auto&& r)
// {
//     std::vector<int> counts(r.front().size(), 0);
//     for (auto&& inner : r) {
//         std::size_t i{0};
//         for (int bit : inner) {
//             counts[i++] += bit;
//         }
//     }
//     return counts;
// }

aoc::solution_result day03(std::string_view input)
{
    const auto position_count{sv_lines(input).front().size()};
    const std::vector<int> ints{sv_lines(input) | rv::transform(binary_to_int) |
                                r::to<std::vector>};
    const auto bit_ranges{ints | rv::transform([=](int i) {
                              return bit_range(i, position_count);
                          })};
    const int rows_count{static_cast<int>(ints.size())};

    const std::vector<int> zeroes(position_count, 0);
    const auto counts{r::accumulate(bit_ranges, zeroes, add_rows)};
    // const auto counts{count_bits(bit_ranges)};

    const auto most_common_bits{
        counts | rv::transform([=](int i) { return i > (rows_count / 2); })};

    const int gamma_rate{binary_range_to_int(most_common_bits)};
    const int mask{~(-1 << position_count)};
    const int epsilon_rate_int{(~gamma_rate) & mask};
    const int power_consumption{gamma_rate * epsilon_rate_int};

    using bit_criteria_func = int (*)(int zero_count, int one_count);
    auto system_rating{[&](bit_criteria_func bit_criteria) {
        auto system_ints{ints};
        for (std::size_t i{0}; i < position_count; i++) {
            auto system_rows_count{system_ints.size()};
            auto bit_pos{position_count - i - 1};
            auto get_bit{[=](int in) { return in >> bit_pos & 1; }};
            auto count_ones{
                r::accumulate(system_ints | rv::transform(get_bit), 0)};
            int count_zeroes{static_cast<int>(system_rows_count) - count_ones};
            int met_criteria{bit_criteria(count_zeroes, count_ones)};
            std::erase_if(system_ints,
                          [=](int in) { return get_bit(in) != met_criteria; });
            if (system_ints.size() == 1) {
                break;
            }
        }

        return system_ints[0];
    }};

    auto most_common{[](int zero_count, int one_count) {
        return zero_count <= one_count ? 0 : 1;
    }};

    auto least_common{[](int zero_count, int one_count) {
        return zero_count > one_count ? 0 : 1;
    }};

    const int oxygen_generator_rating{system_rating(most_common)};
    const int co2_scrubber_rating{system_rating(least_common)};
    const int life_support_rating{oxygen_generator_rating *
                                  co2_scrubber_rating};

    return {power_consumption, life_support_rating};
}

}  // namespace aoc::year2021
