//
// Copyright (c) 2025 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <string_view>
#include <vector>

namespace aoc::year2025 {

namespace {

using Number = int64_t;

char get_digit_or_space(const std::string_view s, std::size_t place)
{
    return place < s.size() ? s[place] : ' ';
}

std::string cephalopod_number_string_for_place(
    const auto& column_of_human_number_fields,
    std::size_t place)
{
    return std::string(
        trim(column_of_human_number_fields |
             rv::transform([place](const std::string_view field) {
                 return get_digit_or_space(field, place);
             }) |
             r::to<std::string>));
}

Number add_column(const std::vector<Number>& column)
{
    return r::accumulate(column, Number{0});
}

Number mul_column(const std::vector<Number>& column)
{
    return r::accumulate(column, Number{1}, std::multiplies<>{});
}

auto human_number_fields_to_cephalopod_numbers(const auto& human_number_fields)
{
    const auto columns_of_cephalopod_number_strings =
        human_number_fields | rv::transform([](const auto& column) {
            return rv::iota(0UZ, 4UZ) |
                   rv::transform([&column](std::size_t place) {
                       return cephalopod_number_string_for_place(column, place);
                   });
        });
    const auto columns_of_cephalopod_numbers =
        columns_of_cephalopod_number_strings |
        rv::transform([](const auto&& column) {
            return column |
                   rv::filter([](const std::string& s) { return !s.empty(); }) |
                   rv::transform(
                       [](const std::string& s) { return to_num<Number>(s); }) |
                   r::to<std::vector>;
        });

    return columns_of_cephalopod_numbers;
}

}  // namespace

aoc::solution_result day06(std::string_view input)
{
    const auto lines = sv_lines(trim(input)) | r::to<std::vector>;
    const auto longest_line_length =
        r::max(lines | rv::transform(
                           [](std::string_view line) { return line.size(); }));

    const std::string_view last_line = lines.back();
    const auto ops_by_index =
        last_line | rv::enumerate |
        rv::filter([](auto p) { return p.second != ' '; }) | r::to<std::vector>;
    const auto op_char_indexes = ops_by_index | rv::keys;
    const auto ops =
        ops_by_index | rv::values | rv::transform([](const char op) {
            return op == '+' ? add_column : mul_column;
        });
    const std::size_t column_count = ops_by_index.size();

    const auto column_fields =
        rv::concat(op_char_indexes, rv::single(longest_line_length + 1)) |
        rv::sliding(2) | rv::transform([](auto&& rng) {
            return std::make_pair(rng.front(), rng.back() - rng.front() - 1);
        }) |
        r::to<std::vector>;

    const auto split_line_by_fields = [&column_fields](std::string_view line) {
        return column_fields | rv::transform([line](auto&& field) {
                   return line.substr(field.first, field.second);
               });
    };

    const auto number_lines = lines | rv::drop_last(1);
    const auto rows_of_human_number_fields =
        number_lines | rv::transform(split_line_by_fields) | r::to<std::vector>;
    const auto columns_of_human_number_fields =
        rv::iota(0UZ, column_count) |
        rv::transform([&rows_of_human_number_fields](std::size_t col) {
            return rows_of_human_number_fields |
                   rv::transform(
                       [col](const auto& line) { return line[col]; }) |
                   r::to<std::vector>;
        }) |
        r::to<std::vector>;

    auto columns_of_human_numbers =
        columns_of_human_number_fields | rv::transform([](auto&& column) {
            return column | rv::transform([](const std::string_view s) {
                       return to_num<Number>(trim(s));
                   }) |
                   r::to<std::vector>;
        });

    const auto accumulate_numbers = [&ops](auto&& column) {
        return r::accumulate(
            rv::zip(column, ops) | rv::transform([](const auto& pair) {
                return pair.second(pair.first);
            }),
            Number{0});
    };

    const auto a = accumulate_numbers(columns_of_human_numbers);

    const auto columns_of_cephalopod_numbers =
        human_number_fields_to_cephalopod_numbers(
            columns_of_human_number_fields);
    const auto b = accumulate_numbers(columns_of_cephalopod_numbers);

    return {a, b};
}

}  // namespace aoc::year2025
