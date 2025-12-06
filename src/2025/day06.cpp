//
// Copyright (c) 2025 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <algorithm>
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

}  // namespace

aoc::solution_result day06(std::string_view input)
{
    const auto lines = sv_lines(trim(input)) | r::to<std::vector>;
    const auto number_lines = lines | rv::drop_last(1);
    auto lines_of_words = lines | rv::transform([](std::string_view line) {
                              return sv_words(line) | r::to<std::vector>;
                          }) |
                          r::to<std::vector>;
    const auto line_count = lines_of_words.size();
    const auto column_count = lines_of_words.front().size();
    const auto numbers_per_column = line_count - 1;

    const auto& lines_of_number_words =
        lines_of_words | rv::take(numbers_per_column);

    auto columns_of_number_words =
        rv::iota(0UZ, column_count) |
        rv::transform([lines_of_number_words](std::size_t col) {
            return lines_of_number_words |
                   rv::transform([col](const auto& line) { return line[col]; });
        });

    auto columns_of_human_numbers =
        columns_of_number_words | rv::transform([](auto&& column) {
            return column | rv::transform([](std::string_view s) {
                       return to_num<Number>(s);
                   }) | r::to<std::vector>;
        });

    using ColumnType = std::vector<Number>;
    auto add_column = +[](const ColumnType& column) {
        return r::accumulate(column, Number{0});
    };
    auto mul_column = +[](const ColumnType& column) {
        return r::accumulate(column, Number{1}, std::multiplies<>{});
    };

    const auto& op_words = lines_of_words.back();
    const auto& ops = op_words | rv::transform([&](const std::string_view op) {
                          return op[0] == '+' ? add_column : mul_column;
                      }) |
                      r::to<std::vector>;

    auto a = r::accumulate(rv::zip(columns_of_human_numbers, ops) |
                               rv::transform([](const auto& pair) {
                                   return pair.second(pair.first);
                               }),
                           Number{0});




    const auto longest_line_length =
        r::max(lines | rv::transform(
                           [](std::string_view line) { return line.size(); }));
    const std::string_view last_line = lines.back();
    const auto op_char_indexes =
        last_line | rv::enumerate |
        rv::filter([](auto p) { return p.second != ' '; }) | rv::keys |
        r::to<std::vector>;
    const auto column_fields =
        rv::concat(op_char_indexes, rv::single(longest_line_length + 1)) |
        rv::sliding(2) | rv::transform([](auto&& rng) {
            return std::make_pair(rng.front(), rng.back() - rng.front() - 1);
        }) |
        r::to<std::vector>;
    // const auto column_field_sizes = column_fields | rv::values;

    const auto split_line_by_fields = [&column_fields](std::string_view line) {
        return column_fields | rv::transform([line](auto&& field) {
                   return line.substr(field.first, field.second);
               });
    };

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

    const auto columns_of_cephalopod_number_strings =
        columns_of_human_number_fields | rv::transform([](const auto& column) {
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
                       [](const std::string& s) { return to_num<Number>(s); }) | r::to<std::vector>;
        });

    auto b = r::accumulate(rv::zip(columns_of_cephalopod_numbers, ops) |
                               rv::transform([](const auto& pair) {
                                   return pair.second(pair.first);
                               }),
                           Number{0});

    return {a, b};
}

}  // namespace aoc::year2025
