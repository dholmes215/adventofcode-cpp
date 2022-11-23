//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <ctre.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <string_view>

namespace aoc::year2015 {

namespace {

using code_t = std::uint64_t;
using coord_t = vec2<int>;

coord_t parse_input(std::string_view input)
{
    input = trim(input);
    constexpr auto matcher{ctre::match<
        R"(To continue, please consult the code grid in the manual.  Enter the code at row (\d+), column (\d+).)">};
    if (auto [whole, y, x] = matcher(input); whole) {
        return {to_int(x), to_int(y)};
    }
    throw input_error{fmt::format("failed to parse input: {}", input)};
}

auto code_sequence()
{
    constexpr code_t first{20151125};
    auto generator{[code = first]() mutable {
        const auto out{code};
        code *= 252533;
        code %= 33554393;
        return out;
    }};
    return rv::generate(generator);
}

auto coordinate_sequence()
{
    constexpr coord_t first{1, 1};
    auto generator{[coord = first]() mutable {
        auto out{coord};
        coord += {1, -1};
        if (coord.y == 0) {
            coord = {1, coord.x};
        }
        return out;
    }};
    return rv::generate(generator);
}

}  // namespace

aoc::solution_result day25(std::string_view input)
{
    const auto given_coords{parse_input(input)};

    auto coord_code_sequence{rv::zip(coordinate_sequence(), code_sequence())};

    const auto search_predicate{[given_coords](const auto& pair) {
        return std::get<0>(pair) == given_coords;
    }};

    const auto found{r::find_if(coord_code_sequence, search_predicate)};

    return {std::get<1>(*found), "🎄"};
}

}  // namespace aoc::year2015
