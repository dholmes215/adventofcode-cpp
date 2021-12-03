//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <charconv>
#include <string>
#include <string_view>

namespace aoc::year2015 {

namespace {

int decoded_string_length(std::string_view s)
{
    int count{static_cast<int>(s.size()) - 2};
    auto iter{s.begin() + 1};
    while (iter < s.end() - 1) {
        if (*iter == '\\') {
            iter++;
            const char next{*iter};
            if (next == '\\' || next == '"') {
                count--;
                iter++;
            }
            else if (next == 'x') {
                count -= 3;
                iter += 3;
            }
            else {
                throw input_error{fmt::format(
                    "invalid escape sequence: \\{} (in {})", next, s)};
            }
        }
        else {
            iter++;
        }
    }
    return count;
}

int encoded_string_length(std::string_view s)
{
    // Add 2 for the quotes, and one for each '\' and '"'.
    return 2 + static_cast<int>(s.size()) +
           static_cast<int>(
               r::count_if(s, [](char c) { return c == '\\' || c == '"'; }));
}

// I naively assumed I would need these functions, but I didn't.

// char hex_to_char(const char* c)
// {
//     int out{0};
//     const auto result{std::from_chars(c, c + 2, out, 16)};
//     if (result.ec != std::errc{}) {
//         throw input_error{
//             fmt::format("invalid hex byte: \\x{}", std::string_view{c, 2})};
//     }
//     return static_cast<char>(out);
// }

// std::string decode_string(std::string_view s)
// {
//     std::string out;
//     out.reserve(s.size() - 2);
//     auto iter{s.begin() + 1};
//     while (iter < s.end() - 1) {
//         if (*iter == '\\') {
//             iter++;
//             if (*iter == '\\') {
//                 out += '\\';
//                 iter++;
//             }
//             else if (*iter == '"') {
//                 out += '"';
//                 iter++;
//             }
//             else if (*iter == 'x') {
//                 iter++;
//                 out += hex_to_char(&*iter);
//                 iter += 2;
//             }
//             else {
//                 throw input_error{fmt::format(
//                     "invalid escape sequence: \\{} (in {})", *iter, s)};
//             }
//         }
//         else {
//             out += *iter++;
//         }
//     }
//     return out;
// }

}  // namespace

aoc::solution_result day08(std::string_view input)
{
    const auto lines{sv_lines(input) | r::to<std::vector>};
    const auto line_size{
        [](std::string_view s) { return static_cast<int>(s.size()); }};
    const auto unparsed_count{
        r::accumulate(lines | rv::transform(line_size), 0)};

    const auto decoded_char_count{
        r::accumulate(lines | rv::transform(decoded_string_length), 0)};
    const auto part_a{unparsed_count - decoded_char_count};

    const auto encoded_char_count{
        r::accumulate(lines | rv::transform(encoded_string_length), 0)};
    const auto part_b{encoded_char_count - unparsed_count};

    return {part_a, part_b};
}

}  // namespace aoc::year2015
