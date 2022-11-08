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
#include <set>
#include <utility>
#include <vector>

using namespace aoc;

const std::vector<std::pair<char, char>> figure20_3{
    {'r', 's'}, {'r', 't'}, {'r', 'w'}, {'s', 'u'}, {'s', 'v'},
    {'t', 'u'}, {'u', 'y'}, {'v', 'w'}, {'v', 'y'}, {'w', 'x'},
    {'w', 'z'}, {'x', 'y'}, {'x', 'z'}};

const std::set<std::vector<char>> figure20_3_all_paths{
    {'s', 'r', 't', 'u', 'y', 'v', 'w', 'x', 'z'},
    {'s', 'r', 't', 'u', 'y', 'v', 'w', 'z'},
    {'s', 'r', 't', 'u', 'y', 'x', 'w', 'z'},
    {'s', 'r', 't', 'u', 'y', 'x', 'z'},
    {'s', 'r', 'w', 'v', 'y', 'x', 'z'},
    {'s', 'r', 'w', 'x', 'z'},
    {'s', 'r', 'w', 'z'},
    {'s', 'u', 't', 'r', 'w', 'v', 'y', 'x', 'z'},
    {'s', 'u', 't', 'r', 'w', 'x', 'z'},
    {'s', 'u', 't', 'r', 'w', 'z'},
    {'s', 'u', 'y', 'v', 'w', 'x', 'z'},
    {'s', 'u', 'y', 'v', 'w', 'z'},
    {'s', 'u', 'y', 'x', 'w', 'z'},
    {'s', 'u', 'y', 'x', 'z'},
    {'s', 'v', 'w', 'r', 't', 'u', 'y', 'x', 'z'},
    {'s', 'v', 'w', 'x', 'z'},
    {'s', 'v', 'w', 'z'},
    {'s', 'v', 'y', 'u', 't', 'r', 'w', 'x', 'z'},
    {'s', 'v', 'y', 'u', 't', 'r', 'w', 'z'},
    {'s', 'v', 'y', 'x', 'w', 'z'},
    {'s', 'v', 'y', 'x', 'z'}};

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
    REQUIRE(path == std::vector<char>{'s', 'v', 'w', 'z'});
}

// TODO: Less code duplication
TEST_CASE("DFS with map/char and lambda", "[bfs]")
{
    auto swap_pair{[](std::pair<char, char> p) {
        return std::pair<char, char>{p.second, p.first};
    }};
    const std::multimap<char, char> adj{
        rv::concat(figure20_3, figure20_3 | rv::transform(swap_pair)) |
        r::to<std::multimap>};
    auto adj_func{[&](char c) { return multimap_value_range(adj, c); }};

    auto path{dfs_path(adj_func, 's', 'z')};

    // XXX See comment in BFS test
    REQUIRE(path == std::vector<char>{'s', 'u', 'y', 'v', 'w', 'x', 'z'});
}

// TODO: Less code duplication
TEST_CASE("backtrack with map/char and lambda", "[backtrack]")
{
    auto swap_pair{[](std::pair<char, char> p) {
        return std::pair<char, char>{p.second, p.first};
    }};
    const std::multimap<char, char> adj{
        rv::concat(figure20_3, figure20_3 | rv::transform(swap_pair)) |
        r::to<std::multimap>};
    auto adj_func{[&](char c) { return multimap_value_range(adj, c); }};

    std::set<std::vector<char>> result;
    auto output_func{[&](const std::vector<char>& c) { result.insert(c); }};

    struct backtrack_graph {
        using vertex_type = char;
        using candidate_type = std::vector<vertex_type>;
        candidate_type root() const { return {'s'}; }
        bool reject(candidate_type& c) const
        {
            return r::count(c, c.back()) > 1;
        }
        bool accept(candidate_type& c) const { return c.back() == 'z'; }
        decltype(adj_func)& adjacencies;
        decltype(output_func)& output;
    };
    backtrack_graph g{adj_func, output_func};
    backtrack(g);

    REQUIRE(result == figure20_3_all_paths);
}

// TODO: Less code duplication
TEST_CASE("backtrack_coro with map/char and lambda", "[backtrack]")
{
    auto swap_pair{[](std::pair<char, char> p) {
        return std::pair<char, char>{p.second, p.first};
    }};
    const std::multimap<char, char> adj{
        rv::concat(figure20_3, figure20_3 | rv::transform(swap_pair)) |
        r::to<std::multimap>};
    auto adj_func{[&](char c) { return multimap_value_range(adj, c); }};

    struct backtrack_graph {
        using vertex_type = char;
        using candidate_type = std::vector<vertex_type>;
        candidate_type root() const { return {'s'}; }
        bool reject(candidate_type& c) const
        {
            // Don't revisit a vertex
            return r::count(c, c.back()) > 1;
        }
        bool accept(candidate_type& c) const { return c.back() == 'z'; }
        decltype(adj_func)& adjacencies;
    };
    backtrack_graph g{adj_func};

    auto result{backtrack_coro(g) | r::to<std::set>};

    REQUIRE(result == figure20_3_all_paths);
}
