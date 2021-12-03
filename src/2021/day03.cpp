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

std::vector<int> sv_to_vector(std::string_view sv)
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
    const auto rows{sv_lines(input) | rv::transform(sv_to_vector) |
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

    return {power_consumption, 0};
}

}  // namespace aoc::year2021
