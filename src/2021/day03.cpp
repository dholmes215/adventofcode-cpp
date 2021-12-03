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

int digit_to_int(char c)
{
    return c - '0';
}

std::vector<int> sv_to_ints(std::string_view sv)
{
    return sv | rv::transform(digit_to_int) | r::to<std::vector>;
}

std::vector<int> add_rows(const std::vector<int>& row1,
                          const std::vector<int>& row2)
{
    std::vector<int> result{rv::zip_with(std::plus<int>(), row1, row2) |
                            r::to<std::vector>};
    return result;
}

int binary_range_to_int(auto&& r)
{
    int result{0};
    for (bool b : r) {
        const int i{b ? 1 : 0};
        result = (result << 1) | i;
    }
    return result;
}

std::string binary_range_to_string(auto&& r)
{
    std::string result;
    for (bool b : r) {
        const char c{b ? '1' : '0'};
        result += c;
    }
    return result;
}

}  // namespace

aoc::solution_result day03(std::string_view input)
{
    const auto rows{sv_lines(input) | rv::transform(sv_to_ints) |
                    r::to<std::vector>};
    const auto position_count{rows[0].size()};
    const int rows_count{static_cast<int>(rows.size())};

    const std::vector<int> zeroes(position_count, 0);
    const auto counts{r::accumulate(rows, zeroes, add_rows)};
    const auto most_common_bits{
        counts | rv::transform([=](int i) { return i > (rows_count / 2); })};
    const int gamma_rate{binary_range_to_int(most_common_bits)};
    // const auto gamma_rate_string{binary_range_to_string(gamma_rate)};

    const int mask{~(-1 << position_count)};
    const int epsilon_rate_int{(~gamma_rate) & mask};

    const int power_consumption{gamma_rate * epsilon_rate_int};

    fmt::print("gamma rate: {} {}\n", gamma_rate, epsilon_rate_int);

    // Part B

    int oxygen_generator_rating{};
    {
        std::vector<std::vector<int>> rows_oxygen{rows.begin(), rows.end()};
        for (std::size_t i{0}; i < position_count; i++) {
            auto rows_oxygen_count{rows_oxygen.size()};
            auto count_ones{r::accumulate(
                rows_oxygen | rv::transform([=](const std::vector<int>& r) {
                    return r[i];
                }),
                0)};
            int count_zeroes{static_cast<int>(rows_oxygen_count) - count_ones};
            int most_common{count_ones >= count_zeroes ? 1 : 0};
            std::vector<std::vector<int>> filtered{
                rows_oxygen |
                rv::filter([=](auto&& r) { return r[i] == most_common; }) |
                r::to<std::vector>};
            std::swap(rows_oxygen, filtered);
            fmt::print("{}: filtered down to {} rows\n", i, rows_oxygen.size());
            if (rows_oxygen.size() == 1) {
                break;
            }
        }

        auto selected = rows_oxygen[0];
        oxygen_generator_rating = binary_range_to_int(
            selected | rv::transform([](int i) { return i == 1; }));

        fmt::print("oxygen generator: {}\n", oxygen_generator_rating);
    }

    int co2_scrubber_rating{};
    {
        std::vector<std::vector<int>> rows_oxygen{rows.begin(), rows.end()};
        for (std::size_t i{0}; i < position_count; i++) {
            auto rows_oxygen_count{rows_oxygen.size()};
            auto count_ones{r::accumulate(
                rows_oxygen | rv::transform([=](const std::vector<int>& r) {
                    return r[i];
                }),
                0)};
            int count_zeroes{static_cast<int>(rows_oxygen_count) - count_ones};
            int least_common{count_ones < count_zeroes ? 1 : 0};
            std::vector<std::vector<int>> filtered{
                rows_oxygen |
                rv::filter([=](auto&& r) { return r[i] == least_common; }) |
                r::to<std::vector>};
            std::swap(rows_oxygen, filtered);
            fmt::print("{}: filtered down to {} rows\n", i, rows_oxygen.size());
            if (rows_oxygen.size() == 1) {
                break;
            }
        }

        auto selected = rows_oxygen[0];
        co2_scrubber_rating = binary_range_to_int(
            selected | rv::transform([](int i) { return i == 1; }));

        fmt::print("co2 scrubber: {}\n", co2_scrubber_rating);
    }

    const int life_support_rating{oxygen_generator_rating *
                                  co2_scrubber_rating};

    return {power_consumption, life_support_rating};
}

}  // namespace aoc::year2021
