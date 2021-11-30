//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_HPP
#define AOC_HPP

#include <fmt/core.h>
#include <fmt/format.h>
#include <tl/expected.hpp>

#include <filesystem>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>

namespace aoc {

// Exception thrown by solutions on unexpected input.
class input_error : public std::runtime_error {
   public:
    input_error(std::string msg) : runtime_error{std::move(msg)} {}
};

struct date {
    int year;
    int day;
    friend auto operator<=>(const date& lhs, const date& rhs) = default;
};

struct solution_result {
    std::string part_a;
    std::string part_b;
    solution_result() = default;
    solution_result(auto&& a, auto&& b)
        : part_a{fmt::format("{}", a)}, part_b{fmt::format("{}", b)}
    {
    }
    friend bool operator==(const solution_result& lhs,
                           const solution_result& rhs) = default;
};

using solution_func = solution_result (*)(std::string_view);

struct solution {
    solution_func func;
    std::string label;
};

using ifstream_expected = tl::expected<std::ifstream, std::string>;
ifstream_expected open_file(const std::filesystem::path& datadir,
                            aoc::date date) noexcept;
ifstream_expected open_file(const std::filesystem::path& inputfile) noexcept;

// Read an entire istream into a string.
std::string slurp(std::istream&);

// Return true if `c` is a whitespace character.  Takes a `char` safely without
// a cast unlike std::isspace.
bool is_whitespace(char c);
std::string_view strip(std::string_view s) noexcept;

}  // namespace aoc

// Custom formatter for aoc::solution_id
template <>
struct fmt::formatter<aoc::date> : public formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const aoc::date& date, FormatContext& ctx)
    {
        const auto out{
            fmt::format("Year {:04} Day {:02}", date.year, date.day)};
        return formatter<std::string_view>::format(out, ctx);
    }
};

#endif  // AOC_HPP
