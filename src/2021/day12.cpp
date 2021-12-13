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

bool is_small(cave_t cave)
{
    return cave != "start" &&
           r::all_of(cave, [](char c) { return c >= 'a' && c <= 'z'; });
}

int count_all_paths(const path_map_t& graph,
                    bool cave_revisit_available = false,
                    cave_t start = "start",
                    cave_t end = "end",
                    std::set<cave_t> visited_small_caves = {})
{
    if (start == end) {
        return 1;
    }

    if (is_small(start)) {
        if (visited_small_caves.contains(start)) {
            cave_revisit_available = false;
        }
        visited_small_caves.insert(start);
    }

    int paths_out{0};

    const auto& adjacent_vertexes{graph.at(start)};
    for (const cave_t& v : adjacent_vertexes) {
        if (is_small(v) && visited_small_caves.contains(v) &&
            !cave_revisit_available) {
            continue;
        }

        paths_out += count_all_paths(graph, cave_revisit_available, v, end,
                                     visited_small_caves);
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
    return {count_all_paths(edge_map, false), count_all_paths(edge_map, true)};
}

}  // namespace aoc::year2021
