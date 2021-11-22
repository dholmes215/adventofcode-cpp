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
#include <range/v3/view/subrange.hpp>
#include <tl/expected.hpp>

#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <string_view>

namespace aoc {

struct date {
    int year;
    int day;
    friend auto operator<=>(const date& lhs, const date& rhs) = default;
};

struct solution_result {
    std::string part_a;
    std::string part_b;
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

// Read an entire istream into a string.
std::string slurp(std::istream&);

// Helper to allow iterating the `char`s in an `istream` with a ranged for loop.
struct istream_range {
   public:
    using Iterator = std::istreambuf_iterator<char>;
    istream_range(std::istream& stream) : begin_iter(stream) {}
    Iterator begin() const noexcept { return begin_iter; }
    Iterator end() const noexcept { return {}; }

   private:
    Iterator begin_iter{};
};

// Return true if `c` is a whitespace character.  Takes a `char` safely without
// a cast unlike std::isspace.
bool is_whitespace(char c);

// Given a std::map<Key,Value>, return the subset of the map ranging from
// first_key to last_key.
template <typename Key, typename Value>
auto submap(const std::map<Key, Value>& map,
            const Key& first_key,
            const Key& last_key)
{
    return ranges::subrange(map.lower_bound(first_key),
                            map.upper_bound(last_key));
}

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
