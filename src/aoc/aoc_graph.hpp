//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_GRAPH_HPP
#define AOC_GRAPH_HPP

#include <cstdint>
#include <map>
#include <optional>
#include <queue>

namespace aoc {

// TODO: Consider making this more generic
// - Replace `std::map`s with type parameters, or use a visitor, or something

// DFS Variations:
// Return breadth-first-tree, or just a (optional) path?
//   bfs_tree()
//   bfs_path()
// Stop at a given destination, or explore entire graph, or stop at a given
// depth?

/// @brief Implementation of Breadth-First Search based on _Introduction to
/// Algorithms, 4th Edition_.
/// @tparam Adjacencies Callable object type which takes a `Vertex` parameter
/// and returns a range of all other vertexes adjacent to the given vertex.
/// @tparam Vertex Type of the vertexes in the graph.
/// @param adj Instance of the `Adjacencies` function.  This replaces "G" in the
/// CLRS version, which represents the graph but which isn't used for anything
/// except finding the adjacent vertexes.
/// @param source The source `Vertex` from which to begin the search.
/// @param destination The destination `Vertex` to search for, if any.
template <typename Vertex, typename Adjacencies>
[[nodiscard]] std::vector<Vertex> bfs_generic(
    Adjacencies&& adj,
    const Vertex& source,
    const std::optional<Vertex>& destination)
{
    enum class graph_color { white, gray, black };
    using distance = std::uint_fast32_t;
    // const distance infinity_value{std::numeric_limits<distance>::max()};

    using queue = std::queue<Vertex>;

    std::map<Vertex, graph_color> colors;

    // "d" in the CLRS version.  If a vertex is missing from this map, that's
    // equivalent to being "infinity" in the CLRS version.
    std::map<Vertex, distance> distances;

    // "pi" in CLRS version. If a vertex is missing from this map, that's
    // equivalent to being "NIL" in the CLRS version.
    std::map<Vertex, Vertex> predecessors;

    // Here CLRS initializes the colors, distances and predecessors for each
    // vertex.  We don't know the vertexes yet but will take advantage of
    // `std::map`'s zero-initialization for colors and will assume missing
    // predecessors are NIL and missing distances are infinity.

    colors[source] = graph_color::gray;
    distances[source] = 0;
    // predecessors[source] = NIL

    queue q;
    q.push(source);

    bool found_destination{false};

    while (!q.empty() && !found_destination) {
        Vertex u{q.front()};
        q.pop();
        for (const Vertex& v : adj(u)) {
            auto& v_color{colors[v]};
            if (v_color == graph_color::white) {
                v_color = graph_color::gray;
                distances[v] = distances[u] + 1;
                predecessors[v] = u;
                q.push(v);
                if (destination && v == *destination) {
                    found_destination = true;
                }
            }
        }
        colors[u] = graph_color::black;
    }

    std::vector<Vertex> path_to_destination;
    if (found_destination) {
        Vertex v{*destination};
        path_to_destination.push_back(v);
        auto iter{predecessors.find(v)};
        while (iter != predecessors.end()) {
            v = iter->second;
            path_to_destination.push_back(v);
            iter = predecessors.find(v);
        }
        std::reverse(path_to_destination.begin(), path_to_destination.end());
    }
    return path_to_destination;

    // TODO: also figure out a way to output the tree
}

/// @brief Search for the shortest path between two vertexes using breadth-first
/// search.
/// @tparam Adjacencies Callable object type which takes a `Vertex` parameter
/// and returns a range of all other vertexes adjacent to the given vertex.
/// @tparam Vertex Type of the vertexes in the graph.
/// @param adj Instance of the `Adjacencies` function.  This replaces "G" in the
/// CLRS version, which represents the graph but which isn't used for anything
/// except finding the adjacent vertexes.
/// @param source The source `Vertex` from which to begin the search.
/// @param destination The destination `Vertex` to search for.
template <typename Vertex, typename Adjacencies>
[[nodiscard]] std::vector<Vertex> bfs_path(Adjacencies&& adj,
                                           const Vertex& source,
                                           const Vertex& destination)
{
    return bfs_generic(adj, source, {destination});
}

// TODO: bfs_tree, returning the discovered breadth-first tree instead of just
// the path.

}  // namespace aoc

#endif  // AOC_GRAPH_HPP
