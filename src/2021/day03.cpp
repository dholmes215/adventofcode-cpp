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

namespace {
int binary_sv_to_int(std::string_view s)
{
    return to_int_base(s, 2);
}

using count_type = std::int16_t;
constexpr std::size_t pos_max{16};
using count_array = std::array<count_type, pos_max>;

auto bit_range(int i, std::size_t count)
{
    auto generator{
        [=]() mutable { return static_cast<count_type>((i >> --count) & 1); }};
    return rv::generate_n(generator, count);
}

count_array add_rows(count_array row1, int in)
{
    auto bits{bit_range(in, pos_max)};
    r::copy(rv::zip_with(std::plus<count_type>{}, row1, bits), row1.begin());
    return row1;
}
}  // namespace

aoc::solution_result day03(std::string_view input)
{
    const std::vector<int> ints{
        sv_lines(input) | rv::transform(binary_sv_to_int) | r::to<std::vector>};

    const auto position_count{sv_lines(input).front().size()};
    if (position_count > pos_max) {
        throw input_error("Too many bits in input lines");
    }

    const int rows_count{static_cast<int>(ints.size())};
    if (rows_count > std::numeric_limits<count_type>::max()) {
        throw input_error("Too many input rows");
    }

    const count_array counts{r::accumulate(ints, count_array{0}, add_rows)};

    const auto most_common_bits{
        counts | rv::transform([=](int i) { return i > (rows_count / 2); })};

    const int gamma_rate{bool_range_to_int(most_common_bits)};
    const int mask{(1 << position_count) - 1};
    const int epsilon_rate_int{(~gamma_rate) & mask};
    const int power_consumption{gamma_rate * epsilon_rate_int};

    using bit_criteria_func = int (*)(int zero_count, int one_count);
    auto compute_system_rating{[&](bit_criteria_func bit_criteria) {
        auto system_ints{ints};
        for (std::size_t i{0}; i < position_count; i++) {
            const auto system_rows_count{system_ints.size()};
            const auto bit_pos{position_count - i - 1};
            auto get_bit{[=](int in) { return in >> bit_pos & 1; }};
            const auto count_ones{
                r::accumulate(system_ints | rv::transform(get_bit), 0)};
            const int count_zeroes{static_cast<int>(system_rows_count) -
                                   count_ones};
            const int met_criteria{bit_criteria(count_zeroes, count_ones)};
            std::erase_if(system_ints,
                          [=](int in) { return get_bit(in) != met_criteria; });
            if (system_ints.size() == 1) {
                break;
            }
        }

        return system_ints[0];
    }};

    auto most_common{
        [](int zeroes, int ones) { return zeroes <= ones ? 0 : 1; }};

    auto least_common{
        [](int zeroes, int ones) { return zeroes > ones ? 0 : 1; }};

    const int oxygen_generator_rating{compute_system_rating(most_common)};
    const int co2_scrubber_rating{compute_system_rating(least_common)};
    const int life_support_rating{oxygen_generator_rating *
                                  co2_scrubber_rating};

    return {power_consumption, life_support_rating};
}

}  // namespace aoc::year2021
