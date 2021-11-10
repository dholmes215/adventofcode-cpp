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

#include <iostream>
#include <map>
#include <string>
#include <string_view>

namespace aoc {

struct solution_id {
    int year;
    int day;
    friend auto operator<=>(const solution_id& lhs,
                            const solution_id& rhs) = default;
};

struct solution_result {
    std::string part_a;
    std::string part_b;
};

using solution_func = solution_result (*)(std::istream&);

}  // namespace aoc

// Custom formatter for aoc::solution_id
template <>
struct fmt::formatter<aoc::solution_id> : public formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const aoc::solution_id& s, FormatContext& ctx)
    {
        const auto out{fmt::format("Year {:04} Day {:02}", s.year, s.day)};
        return formatter<std::string_view>::format(out, ctx);
    }
};

#endif  // AOC_HPP
