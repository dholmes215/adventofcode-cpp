//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "day06.hpp"

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <fmt/format.h>

#include <regex>
#include <string>
#include <string_view>

namespace aoc::year2015 {

namespace lights {

light_action string_to_action(std::string_view s)
{
    if (s == "turn on") {
        return light_action::on;
    }
    else if (s == "turn off") {
        return light_action::off;
    }
    else if (s == "toggle") {
        return light_action::toggle;
    }
    throw input_error(fmt::format("unknown light_action [{}]", s));
}

// TODO: CTRE?
instruction string_to_instruction(std::string_view s)
{
    const std::regex r{"(.+) (\\d+),(\\d+) through (\\d+),(\\d+)"};
    std::smatch m;

    const std::string str{s};
    if (!std::regex_match(str, m, r)) {
        throw input_error(fmt::format("invalid rectangle [{}]", s));
    }
    auto action_str{m[1].str()};
    auto x1{std::stoi(m[2].str())};
    auto y1{std::stoi(m[3].str())};
    auto x2{std::stoi(m[4].str())};
    auto y2{std::stoi(m[5].str())};
    return {string_to_action(action_str),
            {{x1, y1}, {x2 - x1 + 1, y2 - y1 + 1}}};
}

}  // namespace lights

using namespace lights;

void apply_instruction(auto& lights, const instruction& i)
{
    auto data = lights.subgrid(i.region).data();
    for (auto& l : data) {
        do_action(l, i.action);
    }
}

void apply_instructions(auto& lights, const auto& instructions)
{
    for (const auto& i : instructions) {
        apply_instruction(lights, i);
    }
}

aoc::solution_result day06(std::string_view input)
{
    const auto instructions{sv_lines(input) |
                            rv::transform(string_to_instruction) |
                            r::to<std::vector>};

    binary_light_grid lights;
    apply_instructions(lights, instructions);

    dimmable_light_grid lights_b;
    apply_instructions(lights_b, instructions);

    auto to_int{[](const dimmable_light l) -> int { return l; }};

    return {r::count(lights.data(), true),
            r::accumulate(lights_b.data() | rv::transform(to_int), 0)};
}

}  // namespace aoc::year2015
