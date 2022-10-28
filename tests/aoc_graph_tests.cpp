//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc_graph.hpp>
#include <aoc_range.hpp>

#include <catch2/catch_all.hpp>

#include <map>
#include <utility>
#include <vector>

using namespace aoc;

const std::vector<std::pair<char, char>> figure20_3{
    {'r', 's'}, {'r', 't'}, {'r', 'w'}, {'s', 'u'}, {'s', 'v'},
    {'t', 'u'}, {'u', 'y'}, {'v', 'w'}, {'v', 'y'}, {'w', 'x'},
    {'w', 'z'}, {'x', 'y'}, {'x', 'z'}};

TEST_CASE("BFS with map/char and lambda", "[bfs]")
{
    auto swap_pair{[](std::pair<char, char> p) {
        return std::pair<char, char>{p.second, p.first};
    }};
    const std::multimap<char, char> adj{
        rv::concat(figure20_3, figure20_3 | rv::transform(swap_pair)) |
        r::to<std::multimap>};
    auto adj_func{[&](char c) { return multimap_value_range(adj, c); }};

    auto path{bfs_path(adj_func, 's', 'z')};

    // XXX For any graph there are multiple valid breadth-first trees and
    // multiple valid paths between any two points, but which tree and path you
    // get will be deterministic based on the order in which the vertexes'
    // adjacencies are enumerated.  This is the path you will get with the
    // specific multimap above.  It's different from the path in the textbook,
    // but I'm too lazy to make the multiset reflect the book because it doesn't
    // matter.
    REQUIRE(path == std::vector<char>{'v', 'w', 'z'});
}
