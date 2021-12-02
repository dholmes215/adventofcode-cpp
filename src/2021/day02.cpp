//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <fmt/format.h>

#include <string_view>

namespace aoc::year2021 {

namespace {

enum class action {
    forward,
    down,
    up,
};

action to_action(std::string_view sv)
{
    constexpr std::array<std::string_view, 3> words{"forward", "down", "up"};
    auto found{r::find(words, sv)};
    if (found == words.end()) {
        throw input_error{fmt::format("unrecognized action: {}", sv)};
    }
    return static_cast<action>(found - words.begin());
}

struct command {
    action action_;
    int units_;
};

command to_command(std::string_view line)
{
    const auto words{sv_split_range(line, ' ')};
    std::array<std::string_view, 2> words_array;
    r::copy(words, words_array.begin());
    return {to_action(words_array[0]), to_int(words_array[1])};
}

vec2<int> position_change(command cmd)
{
    std::array<vec2<int>, 3> deltas{
        vec2<int>{0, cmd.units_},  // forward
        vec2<int>{cmd.units_, 0},  // down
        vec2<int>{-cmd.units_, 0}  // up
    };
    return deltas[static_cast<std::size_t>(cmd.action_)];
}

struct submarine_state {
    vec2<int> pos_;
    int aim_{0};
};

void apply_command_b(submarine_state& state, command cmd)
{
    switch (cmd.action_) {
        case action::forward:
            state.pos_.x += cmd.units_;
            state.pos_.y += cmd.units_ * state.aim_;
            return;
        case action::down:
            state.aim_ += cmd.units_;
            return;
        case action::up:
            state.aim_ -= cmd.units_;
            return;
        default:
            std::abort();  // unreachable
    }
}

}  // namespace

aoc::solution_result day02(std::string_view input)
{
    const auto commands{sv_lines(input) | rv::transform(to_command) |
                        r::to<std::vector>};
    const auto changes{commands | rv::transform(position_change)};
    auto pos_a{r::accumulate(changes, vec2<int>{0, 0})};

    submarine_state state_b;
    for (const auto& cmd : commands) {
        apply_command_b(state_b, cmd);
    }

    return {pos_a.x * pos_a.y, state_b.pos_.x * state_b.pos_.y};
}

}  // namespace aoc::year2021
