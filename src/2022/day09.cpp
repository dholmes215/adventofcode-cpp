//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <fmt/format.h>

#include <string_view>
#include <vector>
#include <utility>

namespace aoc::year2022 {

namespace {

using move_t = std::pair<char, int>;
using pos_t = vec2<int>;
using rope_t = std::pair<pos_t, pos_t>;
using rope10_t = std::array<pos_t, 10>;

move_t parse_line(std::string_view line)
{
    return {line[0], to_int(line.substr(2))};
}

pos_t dir_to_step(char c)
{
    switch (c) {
        case 'U':
            return {0, -1};
        case 'D':
            return {0, 1};
        case 'L':
            return {-1, 0};
        case 'R':
            return {1, 0};
    }
    throw input_error(fmt::format("Invalid direction: '{}'", c));
}

auto move_to_steps(const move_t& m)
{
    return rv::repeat_n(dir_to_step(m.first), m.second);
}

std::vector<pos_t> input_to_steps(std::string_view input)
{
    return sv_lines(trim(input)) | rv::transform(parse_line) |
           rv::transform(move_to_steps) | rv::join | r::to<std::vector>;
}

template <typename T>
int sign(T val)
{
    return (T{0} < val) - (val < T{0});
}

}  // namespace

aoc::solution_result day09(std::string_view input)
{
    const auto steps{input_to_steps(input)};

    const auto apply_step{[](pos_t step, const rope_t rope) {
        auto [head, tail]{rope};
        head += step;
        const auto delta{head - tail};
        const pos_t norm{sign(delta.x), sign(delta.y)};
        if (norm != delta) {
            tail += {sign(delta.x), sign(delta.y)};
        }
        return rope_t{head, tail};
    }};

    std::vector<rope_t> rope_positions{1};
    for (const auto& step : steps) {
        rope_positions.push_back(apply_step(step, rope_positions.back()));
    }

    std::vector<pos_t> tail_positions{rope_positions | rv::values |
                                      r::to<std::vector>};
    r::sort(tail_positions);
    tail_positions.erase(r::unique(tail_positions), tail_positions.end());

    // Part 2
    const auto apply_step10{[&](pos_t step, const rope10_t rope) {
        rope10_t out;
        out[0] = rope[0] + step;
        for (std::size_t i{0}; i < 9; i++) {
            auto [head, tail]{apply_step(step, {rope[i], rope[i + 1]})};
            out[i + 1] = tail;
            step = out[i + 1] - rope[i + 1];
        }
        return out;
    }};

    std::vector<rope10_t> rope10_positions{1};
    for (const auto& step : steps) {
        rope10_positions.push_back(apply_step10(step, rope10_positions.back()));
    }

    std::vector<pos_t> tail10_positions{
        rope10_positions |
        rv::transform([](const auto& rope) { return rope.back(); }) |
        r::to<std::vector>};
    r::sort(tail10_positions);
    tail10_positions.erase(r::unique(tail10_positions), tail10_positions.end());

    return {tail_positions.size(), tail10_positions.size()};
}

}  // namespace aoc::year2022
