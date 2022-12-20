//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_graph.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <array>
#include <cstdint>
#include <set>
#include <string_view>
#include <vector>

namespace aoc::year2022 {

namespace {
}  // namespace

using int_t = std::int16_t;
using minute_t = int;
const minute_t minute_deadline1{24};
const minute_t minute_deadline2{32};
enum mineral_t : std::uint8_t { ore, clay, obsidian, geode };
constexpr std::uint8_t mineral_count{4};

struct inventory_t {
    std::array<int_t, mineral_count> minerals;
    std::array<int_t, mineral_count> robots;
    friend auto operator<=>(const inventory_t&, const inventory_t&) = default;
};

const inventory_t initial_inventory{{}, {1, 0, 0, 0}};

using robot_costs_t =
    std::array<std::array<int_t, mineral_count>, mineral_count>;

struct blueprint_t {
    int_t id;
    robot_costs_t costs;
};

blueprint_t parse_blueprint(std::string_view line)
{
    const std::vector<int_t> nums{numbers<int_t>(line) | r::to<std::vector>};
    return {nums[0],
            {{{nums[1], 0, 0, 0},
              {nums[2], 0, 0, 0},
              {nums[3], nums[4], 0, 0},
              {nums[5], 0, nums[6], 0}}}};
}

struct state_t {
    minute_t minute{0};
    inventory_t inv{initial_inventory};

    friend auto operator<=>(const state_t&, const state_t&) = default;
};

const state_t initial_state{};

void run_robots(state_t& state)
{
    auto& inv{state.inv};
    for (std::uint8_t mineral{}; mineral < mineral_count; mineral++) {
        inv.minerals[mineral] += inv.robots[mineral];
    }
    state.minute++;
}

// Spends a minute buying a robot, or returns false without spending any time
bool try_buy_robot(state_t& state,
                   mineral_t robot,
                   const blueprint_t& blueprint)
{
    auto& inv{state.inv};
    const auto robot_costs{blueprint.costs[robot]};
    for (std::uint8_t mineral{}; mineral < mineral_count; mineral++) {
        if (robot_costs[mineral] > inv.minerals[mineral]) {
            // Can't afford the robot.
            return false;
        }
    }

    run_robots(state);
    for (std::uint8_t mineral{}; mineral < mineral_count; mineral++) {
        inv.minerals[mineral] -= robot_costs[mineral];
    }
    inv.robots[robot]++;

    return true;
}

state_t wait_and_buy_robot(state_t state,
                           mineral_t robot,
                           const blueprint_t& blueprint)
{
    while (!try_buy_robot(state, robot, blueprint)) {
        run_robots(state);
    }
    return state;
}

state_t wait_until(state_t state, minute_t end)
{
    while (state.minute < end) {
        run_robots(state);
    }
    return state;
}

auto adj_func_for_blueprint(const blueprint_t& blueprint,
                            minute_t minute_deadline)
{
    // Since we can never make more than one robot per minute, we'll never need
    // more of any kind of robot than the max resource cost of any robot for
    // that particular mineral.
    std::array<int_t, 4> robot_max{};
    for (std::uint8_t robot_mineral{}; robot_mineral < mineral_count;
         robot_mineral++) {
        //
        for (std::uint8_t robot_to_buy{}; robot_to_buy < mineral_count;
             robot_to_buy++) {
            robot_max[robot_mineral] =
                std::max(robot_max[robot_mineral],
                         blueprint.costs[robot_to_buy][robot_mineral]);
        }
    }

    const auto adj_func{[=](const state_t& state) {
        std::vector<state_t> out;
        if (state.minute == minute_deadline) {
            return out;
        }

        if (state.inv.robots[obsidian] > 0 &&
            state.inv.robots[obsidian] < robot_max[obsidian]) {
            out.push_back(wait_and_buy_robot(state, geode, blueprint));
        }
        if (state.inv.robots[clay] > 0 &&
            state.inv.robots[clay] < robot_max[clay]) {
            out.push_back(wait_and_buy_robot(state, obsidian, blueprint));
        }
        out.push_back(wait_and_buy_robot(state, clay, blueprint));
        if (state.inv.robots[ore] < robot_max[ore]) {
            out.push_back(wait_and_buy_robot(state, ore, blueprint));
        }
        if (state.inv.robots[geode] > 0) {
            out.push_back(wait_until(state, minute_deadline));
        }

        return out;
    }};

    return adj_func;
}

std::string format_state(const state_t& state)
{
    return fmt::format("[{} minerals: {} robots: {}]", state.minute,
                       state.inv.minerals, state.inv.robots);
}

std::string format_candidate(const std::vector<state_t>& c)
{
    return fmt::format("{}\n", c | rv::transform(format_state));
}

bool geode_compare(const std::vector<state_t>& c1,
                   const std::vector<state_t>& c2)
{
    return c1.back().inv.minerals[geode] < c2.back().inv.minerals[geode];
}

aoc::solution_result day19(std::string_view input)
{
    const std::vector<blueprint_t> blueprints{sv_lines(trim(input)) |
                                              rv::transform(parse_blueprint) |
                                              r::to<std::vector>};

    auto start_func{[&]() -> std::vector<state_t> { return {initial_state}; }};
    using adj_func_type = decltype(adj_func_for_blueprint({}, 0));
    struct backtrack_graph {
        using vertex_type = state_t;
        using candidate_type = std::vector<vertex_type>;
        decltype(start_func)& root;
        bool reject(const candidate_type& c) const
        {
            bool reject_result{c.back().minute > minute_deadline};
            return reject_result;
        }
        bool accept(const candidate_type& c)
        {
            if (c.back().minute != minute_deadline) {
                return false;
            }

            int_t geode_count{c.back().inv.minerals[geode]};
            if (geode_count < best_geodes) {
                return false;
            }
            if (geode_count > best_geodes) {
                best_candidates.clear();
            }
            best_geodes = geode_count;
            // TODO
            // best_candidates.insert(c);

            return true;
        }
        adj_func_type& adjacencies;
        minute_t minute_deadline;

        backtrack_graph(decltype(start_func)& root_,
                        adj_func_type& adjacencies_,
                        minute_t minute_deadline_)
            : root{root_},
              adjacencies{adjacencies_},
              minute_deadline{minute_deadline_}
        {
        }

        blueprint_t blueprint;
        int_t best_geodes{0};
        std::set<candidate_type> best_candidates;
    };

    int quality_sum{0};
    for (const auto& blueprint : blueprints) {
        auto adj_func(adj_func_for_blueprint(blueprint, minute_deadline1));
        backtrack_graph g{start_func, adj_func, minute_deadline1};
        auto candidates{backtrack_coro(g)};
        auto most_geodes{r::max(candidates | rv::transform([](const auto& c) {
                                    return c;
                                }) /* | rv::take(100000000)*/,
                                geode_compare)};
        const auto geode_count{most_geodes.back().inv.minerals[geode]};
        fmt::print("Blueprint {}: {}: {}\n", blueprint.id, geode_count,
                   format_candidate(most_geodes));
        const auto quality_level{geode_count * blueprint.id};
        quality_sum += quality_level;
    }

    int_t geode_product{1};
    for (const auto& blueprint : blueprints | rv::take(3)) {
        auto adj_func(adj_func_for_blueprint(blueprint, minute_deadline2));
        backtrack_graph g{start_func, adj_func, minute_deadline2};
        auto candidates{backtrack_coro(g)};
        auto most_geodes{r::max(candidates | rv::transform([](const auto& c) {
                                    return c;
                                }) /* | rv::take(100000000)*/,
                                geode_compare)};
        const auto geode_count{most_geodes.back().inv.minerals[geode]};
        geode_product *= geode_count;
    }

    return {quality_sum, geode_product};
}

}  // namespace aoc::year2022
