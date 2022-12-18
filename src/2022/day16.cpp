//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_graph.hpp>
#include <aoc_range.hpp>

#include <ctre.hpp>

#include <fmt/ranges.h>

#include <cstdint>
#include <map>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace aoc::year2022 {

namespace {

constexpr size_t minute_deadline1{30};
constexpr size_t minute_deadline2{26};

using valve_t = std::size_t;
using valve_name_t = std::string_view;
using valve_set_t = std::uint64_t;
using flow_t = int;

bool set_contains(valve_set_t set, valve_t valve)
{
    return set & (valve_t{1} << valve);
}

void set_insert(valve_set_t& set, valve_t valve)
{
    set |= (valve_t{1} << valve);
}

valve_set_t set_with(valve_set_t set, valve_t valve)
{
    return set | (valve_t{1} << valve);
}

// void set_remove(valve_set_t& set, valve_t valve)
// {
//     set |= ~(valve_t{1} << valve);
// }

Generator<valve_t> set_range(valve_set_t set)
{
    for (valve_t v{0}; v < 64; v++) {
        if (set_contains(set, v)) {
            co_yield v;
        }
    }
}

// std::vector<valve_t> set_vector(valve_set_t set)
// {
//     auto rng{set_range(set)};
//     return rng | r::to<std::vector>;
// }

struct line_parse_t {
    valve_name_t valve;
    flow_t flow;
    std::vector<valve_name_t> adj;
};

struct network_t;

class floyd_warshall_result_t {
   public:
    static constexpr std::size_t infinity{100};
    floyd_warshall_result_t() = default;
    explicit floyd_warshall_result_t(std::size_t vertex_count)
        : dist_{vertex_count, std::vector<std::size_t>(vertex_count, infinity)},
          next_{vertex_count, std::vector<valve_t>(vertex_count, 0)}
    {
    }
    const auto& dist() const { return dist_; }
    const auto& next() const { return next_; }

   private:
    friend floyd_warshall_result_t floyd_warshall(const network_t&);
    std::vector<std::vector<std::size_t>> dist_;
    std::vector<std::vector<valve_t>> next_;
};

struct network_t {
    std::size_t valve_count;
    valve_set_t flow_valves{0};
    std::vector<valve_name_t> names;
    std::vector<valve_set_t> adjs;
    std::vector<flow_t> flows;
    floyd_warshall_result_t shortest_paths;
};

std::vector<valve_name_t> parse_valve_names(
    const std::vector<line_parse_t>& lines)
{
    std::vector<valve_name_t> out{
        lines | rv::transform([](const auto& l) { return l.valve; }) |
        r::to<std::vector>};
    r::sort(out);
    return out;
}

std::vector<valve_name_t> parse_valve_list(std::string_view valves)
{
    const auto truncate{[](std::string_view sv) { return sv.substr(0, 2); }};
    return sv_words(valves) | rv::transform(truncate) | r::to<std::vector>;
}

line_parse_t parse_line(std::string_view line)
{
    constexpr auto matcher{ctre::match<
        R"(Valve (.+) has flow rate=(.+); tunnels? leads? to valves? (.+))">};
    if (auto [whole, valve, flow, rest] = matcher(line); whole) {
        return {valve, to_num<flow_t>(flow), parse_valve_list(rest)};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

floyd_warshall_result_t floyd_warshall(const network_t& network)
{
    floyd_warshall_result_t out{network.valve_count};
    auto& dist = out.dist_;
    auto& next = out.next_;
    for (valve_t u{0}; u < network.valve_count; u++) {
        for (valve_t v : set_range(network.adjs.at(u))) {
            dist[u][v] = 1;
            next[u][v] = v;
        }
    }
    for (valve_t v{0}; v < network.valve_count; v++) {
        dist[v][v] = 0;
        next[v][v] = v;
    }
    for (valve_t k{0}; k < network.valve_count; k++) {
        for (valve_t i{0}; i < network.valve_count; i++) {
            for (valve_t j{0}; j < network.valve_count; j++) {
                if (dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
    return out;
}

network_t parse_network(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | rv::transform(parse_line) |
                     r::to<std::vector>};
    network_t out;
    out.names = parse_valve_names(lines);
    out.valve_count = out.names.size();
    out.adjs.resize(out.valve_count);
    out.flows.resize(out.valve_count);
    const auto names_to_valves{out.names | rv::enumerate |
                               rv::transform([](auto p) {
                                   // Convert from i,name tuple to name,i pair
                                   const auto [i, name]{p};
                                   return std::pair{name, i};
                               }) |
                               r::to<std::map>};

    for (const auto& [valve_name, flow, adj_names] : lines) {
        const valve_t valve{names_to_valves.at(valve_name)};
        valve_set_t adjs{0};
        for (valve_name_t adj_name : adj_names) {
            const valve_t adj{names_to_valves.at(adj_name)};
            set_insert(adjs, adj);
        }
        out.adjs[valve] = adjs;
        out.flows[valve] = flow;
        if (flow > 0) {
            set_insert(out.flow_valves, valve);
        }
    }

    out.shortest_paths = floyd_warshall(out);
    return out;
}

struct state_t {
    valve_t location;
    valve_set_t opened;
    std::size_t minute;
    flow_t flow_so_far;
    // friend bool operator==(const state_t&, const state_t&) = default;
};

flow_t count_flow(const network_t& network, valve_set_t open_valves)
{
    flow_t out{0};
    for (std::size_t v{0}; v < network.valve_count; v++) {
        if (set_contains(open_valves, v)) {
            out += network.flows[v];
        }
    }

    return out;
}

// void print_dot(const network_t& network)
// {
//     fmt::print("{}", "strict graph G {\n");

//     std::vector<std::array<valve_name_t, 2>> pairs;
//     for (valve_t v{0}; v < network.valve_count; v++) {
//         const auto v_name{network.names[v]};
//         fmt::print("  {} [label=\"{}: {}\"]\n", v_name, v_name,
//                    network.flows[v]);
//         for (valve_t u : set_range(network.adjs[v])) {
//             std::array<valve_name_t, 2> name_pair{v_name, network.names[u]};
//             r::sort(name_pair);
//             pairs.push_back(name_pair);
//         }
//     }
//     r::sort(pairs);
//     pairs.erase(r::unique(pairs), pairs.end());
//     for (const auto& [a, b] : pairs) {
//         fmt::print("  {} -- {}\n", a, b);
//     }
//     fmt::print("{}", "}\n");
// }

}  // namespace

aoc::solution_result day16(std::string_view input)
{
    const network_t network{parse_network(input)};
    // print_dot(network);

    const auto adj_func_for_deadline{[&](std::size_t minute_deadline) {
        return [&, minute_deadline](const state_t& state) {
            valve_set_t to_open{network.flow_valves ^ state.opened};
            const flow_t current_flow_per_minute{
                count_flow(network, state.opened)};
            // TODO not new vector
            std::vector<state_t> out{};
            for (valve_t v : set_range(to_open)) {
                if (v == state.location) {
                    // Don't "move to self" unless we're waiting
                    continue;
                }
                std::size_t distance{
                    network.shortest_paths.dist()[state.location][v]};
                int minutes_to_get_here{static_cast<int>(distance)};
                int flow_this_move{current_flow_per_minute *
                                   (minutes_to_get_here + 1)};
                // Can we get there in time?
                if (state.minute + distance + 1 <= minute_deadline) {
                    out.push_back({v, set_with(state.opened, v),
                                   state.minute + distance + 1,
                                   state.flow_so_far + flow_this_move});
                }
            }
            // ...or just wait out the remaining time.
            out.push_back(
                {state.location, state.opened, minute_deadline,
                 state.flow_so_far +
                     current_flow_per_minute *
                         static_cast<flow_t>(minute_deadline - state.minute)});
            return out;
        };
    }};

    auto start_func{[&]() -> std::vector<state_t> { return {{0, 0, 0, 0}}; }};

    const auto adj_func1(adj_func_for_deadline(minute_deadline1));
    const auto adj_func2(adj_func_for_deadline(minute_deadline2));

    struct backtrack_graph {
        using vertex_type = state_t;
        using candidate_type = std::vector<vertex_type>;
        decltype(start_func)& root;
        bool reject(const candidate_type& /*c*/) const
        {
            // bool reject_result{c.back().minute > minute_deadline};
            // With current code we should never reject a candidate
            return false;
        }
        bool accept(const candidate_type& c) const
        {
            return c.back().minute == minute_deadline;
        }
        decltype(adj_func2)& adjacencies;
        std::size_t minute_deadline;
    };
    backtrack_graph g1{start_func, adj_func1, minute_deadline1};
    auto all_paths1{backtrack_coro(g1)};

    const auto part1_candidate_best_flow{
        r::max(all_paths1, [](const auto& a, const auto& b) {
            return a.back().flow_so_far < b.back().flow_so_far;
        })};
    const flow_t part1_flow{part1_candidate_best_flow.back().flow_so_far};

    backtrack_graph g2{start_func, adj_func2, minute_deadline2};
    auto all_paths2{backtrack_coro(g2)};

    // Pairs of valve sets and associated flows
    const auto path_map_entry{[](const auto& candidate) {
        return std::pair{candidate.back().opened, candidate.back().flow_so_far};
    }};
    std::unordered_map<valve_set_t, flow_t> best_flow_per_valve_set;
    for (const auto& [valves_opened, flow] :
         all_paths2 | rv::transform(path_map_entry)) {
        auto& best_flow{best_flow_per_valve_set[valves_opened]};
        best_flow = std::max(best_flow, flow);
    }

    flow_t best_total_flow{0};

    for (const auto& [valves_opened, flow] : best_flow_per_valve_set) {
        // Find the best complement for this set of valves
        flow_t best_complement_flow{0};
        for (const auto& [valves_opened_complement, flow_complement] :
             best_flow_per_valve_set) {
            if (valves_opened & valves_opened_complement) {
                // This valve set is not a complement
                continue;
            }
            best_complement_flow =
                std::max(best_complement_flow, flow_complement);
        }
        const flow_t combined_flow{best_complement_flow + flow};
        best_total_flow = std::max(best_total_flow, combined_flow);
    }

    return {part1_flow, best_total_flow};
}
}  // namespace aoc::year2022
