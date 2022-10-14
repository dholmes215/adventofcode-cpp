//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <ctre.hpp>

#include <fmt/ranges.h>

#include <array>
#include <cstdint>
#include <functional>
#include <string_view>

namespace aoc::year2015 {

namespace {

using property_t = std::int32_t;
using properties_t = std::array<property_t, 5>;
using teaspoons_t = std::int8_t;

properties_t parse_line(std::string_view line)
{
    constexpr auto matcher{ctre::match<
        R"((.+): capacity (.+), durability (.+), flavor (.+), texture (.+), calories (.+))">};
    if (auto [whole, name, capacity, durability, flavor, texture, calories] =
            matcher(line);
        whole) {
        return {to_num<property_t>(capacity), to_num<property_t>(durability),
                to_num<property_t>(flavor), to_num<property_t>(texture),
                to_num<property_t>(calories)};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

// XXX Should probably use strong types for these instead of providing operator*
// for standard-ish types
properties_t operator*(const properties_t& lhs, const teaspoons_t rhs)
{
    properties_t out{lhs};
    r::transform(out, out.begin(), [=](const property_t p) { return p * rhs; });
    return out;
}

[[maybe_unused]] properties_t operator*(const teaspoons_t lhs,
                                        const properties_t& rhs)
{
    return rhs * lhs;
}

}  // namespace

aoc::solution_result day15(std::string_view input)
{
    const auto add_properties{
        [](const properties_t& lhs, const properties_t& rhs) {
            properties_t out;
            const auto plus{[](property_t a, property_t b) { return a + b; }};
            r::copy(rv::zip_with(plus, lhs, rhs), out.begin());
            return out;
        }};
    const auto base_properties{sv_lines(trim(input)) |
                               rv::transform(parse_line) | r::to<std::vector>};
    if (base_properties.size() != 4) {
        throw input_error(
            "Presently only input size of four ingredients is supported");
    }
    auto modified_properties{base_properties};

    property_t part1_score{0};
    property_t part2_score{0};
    for (teaspoons_t a{0}; a <= 100; a++) {
        for (teaspoons_t b{0}; b <= 100 - a; b++) {
            for (teaspoons_t c{0}; c <= 100 - a - b; c++) {
                const teaspoons_t d{static_cast<teaspoons_t>(100 - a - b - c)};
                const std::array<teaspoons_t, 4> proportions{a, b, c, d};

                // Reuse this vector to avoid reallocation
                for (std::size_t i{0}; i < 4; i++) {
                    modified_properties[i] =
                        base_properties[i] * proportions[i];
                }

                const properties_t combined_ingredients{r::accumulate(
                    modified_properties, properties_t{}, add_properties)};
                const auto this_score{r::accumulate(
                    combined_ingredients | rv::take(4) |
                        rv::transform(
                            [](const property_t p) { return std::max(p, 0); }),
                    property_t{1}, std::multiplies<property_t>{})};
                part1_score = std::max(part1_score, this_score);

                // Calories
                if (combined_ingredients[4] == 500) {
                    part2_score = std::max(part2_score, this_score);
                }
            }
        }
    }

    return {part1_score, part2_score};
}

}  // namespace aoc::year2015
