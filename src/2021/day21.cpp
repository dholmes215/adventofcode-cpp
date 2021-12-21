//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <ctre.hpp>

#include <array>
#include <cstdint>
#include <map>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

namespace {

}  // namespace

using player_t = int;
using position_t = int;
using roll_t = int;
using score_t = int;

class player_state {
   public:
    player_state(player_t player, position_t position) noexcept
        : player_{player}, position_{position}, score_{0}
    {
    }

    player_t player() const noexcept { return player_; }
    position_t position() const noexcept { return position_; }
    score_t score() const noexcept { return score_; }

    void advance(roll_t roll) noexcept
    {
        position_ += roll;
        while (position_ > 10) {
            position_ -= 10;
        }
        score_ += position_;
    }

    bool has_won() const noexcept { return score() >= 1000; }

   private:
    player_t player_;
    position_t position_;
    score_t score_;
};

class die_t {
   public:
    roll_t side() const noexcept { return side_; }
    roll_t roll_count() const noexcept { return roll_count_; }
    operator score_t() const noexcept { return side(); }

    roll_t roll() noexcept
    {
        roll_t out = side_++;
        if (side_ > 100) {
            side_ = 1;
        }
        roll_count_++;
        return out;
    }

   private:
    roll_t side_{1};
    roll_t roll_count_{0};
};

player_state parse_starting_position(std::string_view line)
{
    constexpr auto matcher{
        ctre::match<R"(Player (\d+) starting position: (\d+))">};
    if (auto [whole, player, position] = matcher(line); whole) {
        return {to_num<player_t>(player), to_num<position_t>(position)};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

player_t play_game(std::vector<player_state>& players, die_t& die)
{
    auto roll_three{[&die]() { return die.roll() + die.roll() + die.roll(); }};
    while (true) {
        for (player_state& player : players) {
            roll_t rolls{roll_three()};
            player.advance(rolls);
            if (player.has_won()) {
                return player.player() == 1 ? 2 : 1;  // loser
            }
        }
    }
}

// Part 2

constexpr score_t win_score_b{21};

struct player_state_b {
    position_t position_;
    score_t score_;

    friend auto operator<=>(const player_state_b& lhs,
                            const player_state_b& rhs) noexcept = default;
};

struct game_state_b {
    int turn{0};
    std::array<player_state_b, 2> players{};

    friend auto operator<=>(const game_state_b& lhs,
                            const game_state_b& rhs) noexcept = default;

    bool is_done() const noexcept { return is_won() || is_lost(); }
    bool is_won() const noexcept { return players[0].score_ >= win_score_b; }
    bool is_lost() const noexcept { return players[1].score_ >= win_score_b; }
};

void print_positions(const game_state_b& state)
{
    fmt::print("{:>2} {:>2} {:>2}\n", state.turn, state.players[0].position_,
               state.players[1].position_);
}

using universe_count_t = std::uint64_t;
struct three_roll_frequency {
    int roll;
    universe_count_t count;
};

constexpr std::array<three_roll_frequency, 7> three_roll_frequencies{{
    {3, 1},
    {4, 3},
    {5, 6},
    {6, 7},
    {7, 6},
    {8, 3},
    {9, 1},
}};

using count_map_t = std::map<game_state_b, universe_count_t>;

universe_count_t solve_part2(game_state_b starting_state)
{
    universe_count_t won_count{0};

    count_map_t last_turn_states;
    last_turn_states[starting_state] = 1;

    auto is_done = [](const auto& pair) { return pair.first.is_done(); };
    auto is_won = [](const auto& pair) { return pair.first.is_won(); };

    for (int turn{1}; turn <= 22; turn++) {
        count_map_t turn_states;
        // For each turn state, construct seven new turn states for the 7
        // possible 3-roll outcomes, with the appropriate counts
        for (const auto& [state, count] : last_turn_states) {
            for (const auto& [roll, roll_count] : three_roll_frequencies) {
                game_state_b new_state{state};
                new_state.turn = turn;
                const std::size_t player{
                    static_cast<std::size_t>((new_state.turn + 1) % 2)};

                new_state.players[player].position_ += roll;
                while (new_state.players[player].position_ > 10) {
                    new_state.players[player].position_ -= 10;
                }

                new_state.players[player].score_ +=
                    new_state.players[player].position_;

                turn_states[new_state] += (count * roll_count);
            }
        }

        universe_count_t won_this_turn{
            r::accumulate(turn_states | rv::filter(is_won) | rv::values,
                          universe_count_t{0})};
        won_count += won_this_turn;
        std::erase_if(turn_states, is_done);

        last_turn_states = std::move(turn_states);
    }

    return won_count;
}

aoc::solution_result day21(std::string_view input)
{
    const auto players_start{sv_lines(input) |
                             rv::transform(parse_starting_position) |
                             r::to<std::vector>};
    auto players1{players_start};

    die_t die;
    player_t loser{play_game(players1, die)};

    int part1_result{players1[static_cast<std::size_t>(loser - 1)].score() *
                     die.roll_count()};

    game_state_b state;
    state.players[0] = player_state_b{players_start[0].position(), 0};
    state.players[1] = player_state_b{players_start[1].position(), 0};
    universe_count_t part2_result{solve_part2(state)};

    return {part1_result, part2_result};
}

}  // namespace aoc::year2021
