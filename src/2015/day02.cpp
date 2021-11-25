//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>

#include <fmt/core.h>
#include <fmt/format.h>

#include <range/v3/all.hpp>

#include <array>
#include <charconv>
#include <string_view>

namespace aoc::year2015 {

using ranges::accumulate;
using ranges::to;
using ranges::views::transform;

// TODO: use a units library
using foot = int;
using sqft = int;

// Dimensions of a box (in feet)
struct box {
    foot length;
    foot width;
    foot height;
};

// From
// https://stackoverflow.com/questions/56634507/safely-convert-stdstring-view-to-int-like-stoi-or-atoi
int to_int(const std::string_view& input)
{
    int out;
    const std::from_chars_result result{
        std::from_chars(input.data(), input.data() + input.size(), out)};
    if (result.ec == std::errc::invalid_argument ||
        result.ec == std::errc::result_out_of_range) {
        throw input_error(fmt::format("{} is not a valid integer", input));
    }
    return out;
}

box string_to_box(std::string_view s)
{
    // TODO: validate input
    std::array<foot, 3> a;
    const auto dims{sv_split_range(s, 'x') | transform(to_int)};
    ranges::copy(dims, a.begin());
    return {a[0], a[1], a[2]};
}

// Calculate wrapping paper required for a single box.
sqft paper_required(const box& b) noexcept
{
    const std::array<sqft, 3> side_areas{
        b.length * b.width, b.length * b.height, b.width * b.height};
    const sqft smallest_side = *ranges::min_element(side_areas);
    const sqft paper = side_areas[0] * 2 + side_areas[1] * 2 +
                       side_areas[2] * 2 + smallest_side;
    return paper;
}

// Calculate ribbon required for a single box.
foot ribbon_required(const box& b) noexcept
{
    const std::array<foot, 3> side_perimeters{2 * (b.length + b.width),
                                              2 * (b.length + b.height),
                                              2 * (b.width + b.height)};
    const foot shortest_perimeter = *ranges::min_element(side_perimeters);
    const foot bow = b.length * b.width * b.height;
    const foot ribbon = shortest_perimeter + bow;
    return ribbon;
}

struct paper_and_ribbon {
    sqft paper{0};
    foot ribbon{0};
    friend paper_and_ribbon operator+(const paper_and_ribbon& lhs,
                                      const paper_and_ribbon& rhs) noexcept
    {
        return {lhs.paper + rhs.paper, lhs.ribbon + rhs.ribbon};
    }
};

paper_and_ribbon materials_required(const box& b) noexcept
{
    return {paper_required(b), ribbon_required(b)};
}

aoc::solution_result day02(std::string_view input)
{
    const auto materials =
        accumulate(sv_lines(input) | transform(string_to_box) |
                       transform(materials_required),
                   paper_and_ribbon{});
    return {materials.paper, materials.ribbon};
}

}  // namespace aoc::year2015
