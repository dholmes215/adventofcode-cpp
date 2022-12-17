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
#include <vector>

namespace aoc::year2022 {

namespace {

}  // namespace

constexpr size_t minute_deadline{30};

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

void set_remove(valve_set_t& set, valve_t valve)
{
    set |= ~(valve_t{1} << valve);
}

Generator<valve_t> set_range(valve_set_t set)
{
    for (valve_t v{0}; v < 64; v++) {
        if (set_contains(set, v)) {
            co_yield v;
        }
    }
}

std::vector<valve_t> set_vector(valve_set_t set)
{
    auto rng{set_range(set)};
    return rng | r::to<std::vector>;
}

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
    friend bool operator==(const state_t&, const state_t&) = default;
};

const std::vector<valve_t> example_best{0, 3, 3, 2, 1, 1, 0, 8, 9, 9,
                                        8, 0, 3, 4, 5, 6, 7, 6, 6, 5,
                                        4, 4, 3, 2, 2, 2, 2, 2, 2, 2};

bool example_starts_with(const std::vector<state_t>& candidate)
{
    return r::equal(
        candidate | rv::transform([](const state_t& s) { return s.location; }),
        example_best | rv::take(candidate.size()));
}

std::string format_state(const state_t& s)
{
    return fmt::format("{}:{}", s.location, s.opened);
}

valve_t to_location(const state_t& s)
{
    return s.location;
}

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

flow_t evaluate_flow(const network_t& network,
                     const std::vector<state_t>& candidate)
{
    flow_t out{0};
    for (const auto& state : candidate) {
        out += count_flow(network, state.opened);
    }
    return out;
}

void print_dot(const network_t& network)
{
    fmt::print("{}", "strict graph G {\n");

    std::vector<std::array<valve_name_t, 2>> pairs;
    for (valve_t v{0}; v < network.valve_count; v++) {
        const auto v_name{network.names[v]};
        fmt::print("  {} [label=\"{}: {}\"]\n", v_name, v_name,
                   network.flows[v]);
        for (valve_t u : set_range(network.adjs[v])) {
            std::array<valve_name_t, 2> name_pair{v_name, network.names[u]};
            r::sort(name_pair);
            pairs.push_back(name_pair);
        }
    }
    r::sort(pairs);
    pairs.erase(r::unique(pairs), pairs.end());
    for (const auto& [a, b] : pairs) {
        fmt::print("  {} -- {}\n", a, b);
    }
    fmt::print("{}", "}\n");
}

aoc::solution_result day16(std::string_view input)
{
    const network_t network{parse_network(input)};
    const auto valve_name{[&](const valve_t v) { return network.names[v]; }};
    const auto format_with_name{[&](const state_t& s) {
        return fmt::format("{}:{}", valve_name(s.location), s.opened);
    }};

    // const auto flow_valves_vec{set_vector(network.flow_valves)};

    // fmt::print("Valves to open: {}\n",
    //            flow_valves_vec | rv::transform(valve_name));

    // print_dot(network);
    const auto adj_func{[&](const state_t& state) {
        valve_set_t to_open{network.flow_valves ^ state.opened};

        // const auto open_vec{set_vector(state.opened)};
        // const auto to_open_vec{set_vector(to_open)};
        // fmt::print("Open valves: {}\n", open_vec |
        // rv::transform(valve_name)); fmt::print("Valves to open: {}\n",
        //            to_open_vec | rv::transform(valve_name));

        const auto next_min{state.minute + 1};
        // TODO not new vector
        std::vector<state_t> out{};
        if (network.flows[state.location] > 0 &&
            set_contains(to_open, state.location)) {
            out.push_back({state.location,
                           set_with(state.opened, state.location), next_min});
        }
        valve_set_t neighbors_to_visit{0};
        for (valve_t v : set_range(to_open)) {
            if (v == state.location) {
                // Don't "move to self" unless we're waiting
                continue;
            }
            valve_t next{network.shortest_paths.next()[state.location][v]};
            std::size_t distance{
                network.shortest_paths.dist()[state.location][v]};
            // Can we get there in time?
            // if (v == 7) {
            //     fmt::print("distance to 7: {}\n", distance);
            // }
            if (state.minute + distance <= minute_deadline) {
                set_insert(neighbors_to_visit, next);
            }
        }
        for (valve_t v : set_range(neighbors_to_visit)) {
            out.push_back({v, state.opened, next_min});
        }
        if (out.empty()) {
            // There are no useful moves to take, so just wait out the
            // remaining time.
            auto wait{state};
            wait.minute = next_min;
            out.push_back(wait);
        }
        // fmt::print("Proposed states: {}\n",
        //            out | rv::transform(format_with_name));
        return out;
    }};
    auto start_func{[&]() -> std::vector<state_t> { return {{0, 0, 1}}; }};
    // auto start_func{[&]() -> std::vector<state_t> {
    //     return {{0, 0, 1},    {3, 0, 2},    {3, 8, 3},    {2, 8, 4},
    //             {1, 8, 5},    {1, 10, 6},   {1, 0, 7},    {8, 10, 8},
    //             {9, 10, 9},   {9, 522, 10}, {8, 522, 11}, {0, 522, 12},
    //             {3, 522, 13}, {4, 522, 14}};
    // }};

    struct backtrack_graph {
        using vertex_type = state_t;
        using candidate_type = std::vector<vertex_type>;
        decltype(start_func)& root;
        bool reject(const candidate_type& c) const
        {
            const auto equal_location_and_valves{[&](const state_t s) {
                return s.location == c.back().location &&
                       s.opened == c.back().opened;
            }};
            const bool waiting{c.size() >= 2 &&
                               equal_location_and_valves(c[c.size() - 2])};
            const bool useless_cycle{
                !waiting &&
                (std::find_if(c.crbegin() + 2, c.crend(),
                              equal_location_and_valves) != c.crend())};
            bool reject_result = useless_cycle || (c.size() > minute_deadline);
            // if (reject_result && example_starts_with(c)) {
            //     fmt::print("REJECTING: {}\n", c |
            //     rv::transform(format_state));
            // }
            return reject_result;
        }
        bool accept(const candidate_type& c) const
        {
            // if (example_starts_with(c)) {
            //     fmt::print("CONSIDERING: {}\n",
            //                c | rv::transform(format_state));
            // }
            // if (c.size() == minute_deadline && example_starts_with(c)) {
            //     fmt::print("ACCEPTING: {}\n", c |
            //     rv::transform(format_state));
            // }
            return c.size() == minute_deadline;
        }
        decltype(adj_func)& adjacencies;
    };
    backtrack_graph g{start_func, adj_func};
    auto all_paths{backtrack_coro(g)};

    const auto evaluate_flow1{[&](const auto& candidate) {
        return evaluate_flow(network, candidate);
    }};
    const auto zip_with_flow{[&](const auto& candidate) {
        return std::pair{candidate, evaluate_flow1(candidate)};
    }};
    // for (const auto& p : all_paths) {
    //     fmt::print("{}\n", p | rv::transform(format_state));
    // }
    const auto part1_best_with_flow{r::max(
        all_paths | rv::transform(zip_with_flow),
        [](const auto& a, const auto& b) { return a.second < b.second; })};
    fmt::print("{}\n",
               part1_best_with_flow.first | rv::transform(format_with_name));
    const flow_t part1_flow{part1_best_with_flow.second};
    // const flow_t part1_flow{0};

    return {part1_flow, ""};
}

}  // namespace aoc::year2022
