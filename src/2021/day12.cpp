//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <map>
#include <set>
#include <string_view>
#include <utility>
#include <vector>

namespace aoc::year2021 {

namespace {

using cave_t = std::string_view;
using path_map_t = std::map<cave_t, std::set<cave_t>>;
using path_t = std::vector<cave_t>;

bool is_small(cave_t cave)
{
    return cave != "start" &&
           r::all_of(cave, [](char c) { return c >= 'a' && c <= 'z'; });
}

std::vector<path_t> find_all_paths_a(const path_map_t& graph,
                                     cave_t start = "start",
                                     cave_t end = "end",
                                     std::set<cave_t> visited_small_caves = {})
{
    if (start == end) {
        return {path_t{start}};
    }

    if (is_small(start)) {
        visited_small_caves.insert(start);
    }

    std::vector<path_t> paths_out;

    const auto& adjacent_vertexes{graph.at(start)};
    for (const cave_t& v : adjacent_vertexes) {
        if (is_small(v) && visited_small_caves.contains(v)) {
            continue;
        }

        for (path_t path :
             find_all_paths_a(graph, v, end, visited_small_caves)) {
            path.push_back(start);
            paths_out.push_back(std::move(path));
        }
    }

    return paths_out;
}

std::vector<path_t> find_all_paths_b(
    const path_map_t& graph,
    cave_t start = "start",
    cave_t end = "end",
    std::map<cave_t, int> small_cave_visits = {},
    bool single_small_cave_visited_twice = false)
{
    if (start == end) {
        return {path_t{start}};
    }

    if (is_small(start)) {
        small_cave_visits[start]++;
        if (small_cave_visits[start] == 2) {
            single_small_cave_visited_twice = true;
        }
    }

    std::vector<path_t> paths_out;

    const auto& adjacent_vertexes{graph.at(start)};
    for (const cave_t& v : adjacent_vertexes) {
        if (is_small(v) && small_cave_visits.contains(v) &&
            (small_cave_visits.at(v) >= 2 || single_small_cave_visited_twice)) {
            continue;
        }

        for (path_t path : find_all_paths_b(graph, v, end, small_cave_visits,
                                            single_small_cave_visited_twice)) {
            path.push_back(start);
            paths_out.push_back(std::move(path));
        }
    }

    return paths_out;
}

auto split_line(std::string_view line)
{
    const auto dash{line.find_first_of('-')};
    return std::make_pair(line.substr(0, dash), line.substr(dash + 1));
}

auto reverse_pair(std::pair<cave_t, cave_t> pair)
{
    return std::make_pair(pair.second, pair.first);
}

bool is_start_or_end(std::pair<cave_t, cave_t> pair)
{
    return pair.first == "end" || pair.second == "start";
}

path_map_t build_graph(std::string_view input)
{
    const auto lines{sv_lines(input)};
    const auto edges{lines | rv::transform(split_line) | r::to<std::vector>};
    const auto reversed{edges | rv::transform(reverse_pair)};
    const auto combined{rv::concat(edges, reversed)};
    const auto directed_edges{combined | rv::remove_if(is_start_or_end) |
                              r::to<std::vector>};

    path_map_t edge_map{};
    for (const auto& [from, to] : directed_edges) {
        edge_map[from].insert(to);
    }
    return edge_map;
}

}  // namespace

aoc::solution_result day12(std::string_view input)
{
    const path_map_t edge_map{build_graph(input)};
    return {find_all_paths_a(edge_map).size(),
            find_all_paths_b(edge_map).size()};
}

}  // namespace aoc::year2021
