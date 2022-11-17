//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <array>
#include <string_view>
#include <tuple>
#include <vector>

namespace aoc::year2015 {

namespace {

struct stats {
    int hit_points;
    int damage;
    int armor;
};

stats parse_input(std::string_view input)
{
    const auto lines{sv_lines(input) | r::to<std::vector>};
    return {to_int(lines[0].substr(lines[0].find_last_of(' ') + 1)),
            to_int(lines[1].substr(lines[1].find_last_of(' ') + 1)),
            to_int(lines[2].substr(lines[2].find_last_of(' ') + 1))};
}

struct equipment {
    const char* name;
    int cost;
    int damage;
    int armor;
    friend auto operator<=>(const equipment&,
                            const equipment&) noexcept = default;
};

// clang-format off
const std::array<equipment, 5> weapons{
    {{"Dagger", 8, 4, 0},
     equipment{"Shortsword", 10, 5, 0},
     equipment{"Warhammer", 25, 6, 0},
     equipment{"Longsword", 40, 7, 0},
     equipment{"Greatsword", 74, 8, 0}}};

const std::array<equipment, 5> armor{
    {equipment{"Leather", 13, 0, 1},
     equipment{"Chainmail", 31, 0, 2},
     equipment{"Splintmail", 53, 0, 3},
     equipment{"Bandedmail", 75, 0, 4},
     equipment{"Platemail", 102, 0, 5}}};

const std::array<equipment, 6> rings{
    {{"Damage +1", 25, 1, 0},
     {"Damage +2", 50, 2, 0},
     {"Damage +3", 100, 3, 0},
     {"Defense +1", 20, 0, 1},
     {"Defense +2", 40, 0, 2},
     {"Defense +3", 80, 0, 3}}};
// clang-format on

int equipment_cost(const equipment& e) noexcept
{
    return e.cost;
}

int add_cost(const std::vector<equipment> loadout) noexcept
{
    return r::accumulate(loadout | rv::transform(equipment_cost), 0);
}

template <typename T>
auto single_to_vector(const T& single)
{
    return std::vector<T>{{single}};
}

template <typename T>
auto tuple2_to_vector(const std::tuple<T, T>& pair)
{
    const auto& [first, second]{pair};
    return std::vector<T>{{first, second}};
}

template <typename T>
auto tuple3_joined_to_vector(const std::tuple<T, T, T>& triple)
{
    const auto& [first, second, third]{triple};
    return rv::concat(first, second, third) | r::to<std::vector>;
}

stats calculate_player_stats(const std::vector<equipment>& loadout)
{
    stats out{100, 0, 0};
    for (const equipment& e : loadout) {
        out.damage += e.damage;
        out.armor += e.armor;
    }
    return out;
}

bool do_battle(stats player_stats, stats boss_stats)
{
    const auto player_damage{
        std::max(1, player_stats.damage - boss_stats.armor)};
    const auto boss_damage{std::max(1, boss_stats.damage - player_stats.armor)};
    while (true) {
        boss_stats.hit_points -= player_damage;
        if (boss_stats.hit_points <= 0) {
            return true;  // Player wins
        }
        player_stats.hit_points -= boss_damage;
        if (player_stats.hit_points <= 0) {
            return false;  // Boss wins
        }
    }
}

}  // namespace

aoc::solution_result day21(std::string_view input)
{
    const auto boss_stats{parse_input(trim(input))};

    auto weapon_loadouts{weapons | rv::transform(single_to_vector<equipment>) |
                         r::to<std::vector>};

    auto armor_loadouts{armor | rv::transform(single_to_vector<equipment>) |
                        r::to<std::vector>};
    armor_loadouts.emplace_back();

    auto one_ring_loadouts{rings | rv::transform(single_to_vector<equipment>)};
    auto two_ring_loadouts{
        rv::cartesian_product(rings, rings) |
        rv::transform(tuple2_to_vector<equipment>) |
        rv::filter([](const auto& vec) { return vec[0] < vec[1]; })};
    auto ring_loadouts{rv::concat(one_ring_loadouts, two_ring_loadouts) |
                       r::to<std::vector>};
    ring_loadouts.emplace_back();

    auto loadout_triples{
        rv::cartesian_product(weapon_loadouts, armor_loadouts, ring_loadouts)};
    auto loadout_vectors{
        loadout_triples |
        rv::transform(tuple3_joined_to_vector<std::vector<equipment>>)};

    auto loadouts_by_cost{loadout_vectors |
                          rv::transform([](const std::vector<equipment>& vec) {
                              return std::make_pair(add_cost(vec), vec);
                          }) |
                          r::to<std::vector>};

    r::sort(loadouts_by_cost,
            [](const auto& p1, const auto& p2) { return p1.first < p2.first; });

    const auto battle_func{[&](const auto& loadout_pair) {
        return do_battle(calculate_player_stats(loadout_pair.second),
                         boss_stats);
    }};
    const auto part1_gold{r::find_if(loadouts_by_cost, battle_func)->first};

    const auto part2_gold{
        r::find_if_not(loadouts_by_cost | rv::reverse, battle_func)->first};

    return {part1_gold, part2_gold};
}

}  // namespace aoc::year2015
