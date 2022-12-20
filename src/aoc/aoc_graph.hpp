//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_GRAPH_HPP
#define AOC_GRAPH_HPP

#include <coro_generator.hpp>

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <queue>
#include <vector>

namespace aoc {

/// @brief Adaptation of Print-Path from CLRS page 562.
/// @tparam Vertex Vertex type of the graph.
/// @tparam Pred Function taking a Vertex and returning a std::optional<Vertex>.
/// @tparam Print Function consuming a Vertex to "print" it.
/// @param pred Function taking a Vertex and returing its predecessor if any, or
/// nullopt if none.
/// @param source The source vertex.
/// @param dest The testination vertex.
/// @param print Visitor to "print" each vertex on the path.  Alternatively it
/// could add the vertex to a vector.
template <typename Vertex, typename Pred, typename Print>
void print_path(Pred&& pred,
                const Vertex& source,
                const Vertex& dest,
                Print&& print)
{
    if (dest == source) {
        print(source);
    }
    else {
        const std::optional<Vertex> p{pred(dest)};
        if (!p) {
            // no path from source to dest exists
        }
        else {
            print_path(pred, source, *p, print);
            print(dest);
        }
    }
}

template <typename Vertex, typename Pred>
[[nodiscard]] std::vector<Vertex> get_path(Pred&& pred,
                                           const Vertex& source,
                                           const Vertex& dest)
{
    std::vector<Vertex> out;
    const auto add_to_out{[&](const Vertex& v) { out.push_back(v); }};
    print_path(pred, source, dest, add_to_out);
    return out;
}

// template <typename Map, typename Key, typename Value>
// [[nodiscard]] const Value* try_at(const Map<Key, Value>& map, const Key& key)
// {
//     const auto found{map.find(key)};
//     return found == map.end() ? nullptr : &(found->second);
// }

template <typename Map, typename Key>
[[nodiscard]] auto try_at(const Map& map, const Key& key)
{
    const auto found{map.find(key)};
    return found == map.end()
               ? std::optional<typename Map::mapped_type>{}
               : std::optional<typename Map::mapped_type>{found->second};
}

// TODO: Consider making this more generic
// - Replace `std::map`s with type parameters, or use a visitor, or something

// BFS Variations:
// Return breadth-first-tree, or just a (optional) path?  Or
// just find distance?
//   bfs_tree()
//   bfs_path()
//   bfs_distance()
// Stop at a given destination, or explore entire graph, or stop
// at a given depth?

/// @brief Implementation of Breadth-First Search based on
/// _Introduction to Algorithms, 4th Edition_.
/// @tparam Adjacencies Callable object type which takes a
/// `Vertex` parameter and returns a range of all other vertexes
/// adjacent to the given vertex.
/// @tparam Vertex Type of the vertexes in the graph.
/// @param adj Instance of the `Adjacencies` function.  This
/// replaces "G" in the CLRS version, which represents the graph
/// but which isn't used for anything except finding the
/// adjacent vertexes.
/// @param source The source `Vertex` from which to begin the
/// search.
/// @param destination The destination `Vertex` to search for,
/// if any.
template <typename Vertex, typename Adjacencies>
[[nodiscard]] std::vector<Vertex> bfs_generic(
    Adjacencies&& adj,
    const Vertex& source,
    const std::optional<Vertex>& destination)
{
    enum class graph_color { white, gray, black };
    using distance = std::uint_fast32_t;
    // const distance
    // infinity_value{std::numeric_limits<distance>::max()};

    using queue = std::queue<Vertex>;

    std::map<Vertex, graph_color> colors;

    // "d" in the CLRS version.  If a vertex is missing from
    // this map, that's equivalent to being "infinity" in the
    // CLRS version.
    std::map<Vertex, distance> distances;

    // "pi" in CLRS version. If a vertex is missing from this
    // map, that's equivalent to being "NIL" in the CLRS
    // version.
    std::map<Vertex, Vertex> predecessors;

    // Here CLRS initializes the colors, distances and
    // predecessors for each vertex.  We don't know the vertexes
    // yet but will take advantage of `std::map`'s
    // zero-initialization for colors and will assume missing
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
        const auto get_predecessor{
            [&](const Vertex& v) { return try_at(predecessors, v); }};
        return get_path(get_predecessor, source, *destination);
    }

    return {};

    // TODO: also figure out a way to output the tree
}

// TODO: Combine this version and the previous into one

/// @brief Implementation of Breadth-First Search based on
/// _Introduction to Algorithms, 4th Edition_.
/// @tparam Adjacencies Callable object type which takes a
/// `Vertex` parameter and returns a range of all other vertexes
/// adjacent to the given vertex.
/// @tparam Vertex Type of the vertexes in the graph.
/// @tparam AcceptFunc Predicate taking a Vertex and returning true if the
/// vertex is a valid destination.
/// @param adj Instance of the `Adjacencies` function.  This
/// replaces "G" in the CLRS version, which represents the graph
/// but which isn't used for anything except finding the
/// adjacent vertexes.
/// @param source The source `Vertex` from which to begin the
/// search.
/// @param destination The destination `Vertex` to search for,
/// if any.
template <typename Vertex, typename Adjacencies, typename AcceptFunc>
[[nodiscard]] std::vector<Vertex> bfs_accept(Adjacencies&& adj,
                                             const Vertex& source,
                                             const AcceptFunc& accept)
{
    enum class graph_color { white, gray, black };
    using distance = std::uint_fast32_t;
    // const distance
    // infinity_value{std::numeric_limits<distance>::max()};

    using queue = std::queue<Vertex>;

    std::map<Vertex, graph_color> colors;

    // "d" in the CLRS version.  If a vertex is missing from
    // this map, that's equivalent to being "infinity" in the
    // CLRS version.
    std::map<Vertex, distance> distances;

    // "pi" in CLRS version. If a vertex is missing from this
    // map, that's equivalent to being "NIL" in the CLRS
    // version.
    std::map<Vertex, Vertex> predecessors;

    // Here CLRS initializes the colors, distances and
    // predecessors for each vertex.  We don't know the vertexes
    // yet but will take advantage of `std::map`'s
    // zero-initialization for colors and will assume missing
    // predecessors are NIL and missing distances are infinity.

    colors[source] = graph_color::gray;
    distances[source] = 0;
    // predecessors[source] = NIL

    queue q;
    q.push(source);

    bool found_destination{false};
    Vertex accepted;

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
                if (accept(v)) {
                    found_destination = true;
                    accepted = v;
                }
            }
        }
        colors[u] = graph_color::black;
    }

    std::vector<Vertex> path_to_destination;
    if (found_destination) {
        const auto get_predecessor{
            [&](const Vertex& v) { return try_at(predecessors, v); }};
        return get_path(get_predecessor, source, accepted);
    }

    return {};

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

// DFS variations
// Multiple start vertexes?  The CLRS version knows all vertexes to start and
//   will start multiple times until the graph is fully explored.
// Search for tree or just path?
// Vertexes known at beginning or not?
// Optional depth limit?  Required for infinite graph, component of IDDFS?

/// @brief Implementation of Depth-First Search based on _Introduction to
/// Algorithms, 4th Edition_.
/// @tparam Adjacencies Callable object type which takes a `Vertex` parameter
/// and returns a range of all other vertexes adjacent to the given vertex.
/// @tparam Vertex Type of the vertexes in the graph.
/// @param adj Instance of the `Adjacencies` function.  This replaces "G" in the
/// CLRS version, which represents the graph but which isn't used for anything
/// except finding the adjacent vertexes.
/// @param source The source `Vertex` from which to begin the search.
/// @param destination The destination `Vertex` to search for, if any.
/// @param depth_limit
/// @return Path to destination, if any
template <typename Vertex, typename Adjacencies>
[[nodiscard]] std::vector<Vertex> dfs_single_source(
    Adjacencies&& adj,
    const Vertex& source,
    const std::optional<Vertex>& destination,
    const int depth_limit = 0)
{
    enum class graph_color { white, gray, black };

    std::map<Vertex, graph_color> colors;

    // "d" in the CLRS version.  If a vertex is missing from this map, that's
    // equivalent to being "infinity" in the CLRS version.
    std::map<Vertex, std::uint64_t> distances;

    // "pi" in CLRS version. If a vertex is missing from this map, that's
    // equivalent to being "NIL" in the CLRS version.
    std::map<Vertex, Vertex> predecessors;
    std::map<Vertex, std::uint64_t> finish_times;

    std::uint64_t time{0};  // "global" variable used for timestamping

    // XXX This is a good example of where code would be much uglier without a
    // closure: CLRS uses a "global" variable that we want to be local to DFS,
    // and we want to store those maps separately, and it would be ugly to have
    // to pass them all as reference parameters.
    // FIXME There are better ways to make a recursive lambda without the
    // overhead of std::function.
    std::function<void(Adjacencies&&, const Vertex&, int)> dfs_visit;
    dfs_visit = [&](Adjacencies&& adj2, const Vertex& u, int depth) {
        if (depth_limit != 0 && depth >= depth_limit) {
            return;
        }
        time++;
        distances[u] = time;
        colors[u] = graph_color::gray;

        for (const Vertex& v : adj2(u)) {
            if (colors[v] == graph_color::white) {
                predecessors[v] = u;
                dfs_visit(adj2, v, depth + 1);
            }
        }
        time++;
        finish_times[u] = time;
        colors[u] = graph_color::black;
    };

    // CLRS iterates all white vertexes in the whole graph here, but for
    // now I'm just using a single source.
    dfs_visit(adj, source, 0);

    std::vector<Vertex> path_to_destination;
    if (destination) {
        const auto get_predecessor{
            [&](const Vertex& v) { return try_at(predecessors, v); }};
        return get_path(get_predecessor, source, *destination);
    }

    return {};

    // TODO: also figure out a way to output the forest
}

template <typename Vertex, typename Adjacencies>
[[nodiscard]] std::vector<Vertex> dfs_path(Adjacencies&& adj,
                                           const Vertex& source,
                                           const Vertex& destination,
                                           const int depth_limit = 0)
{
    return dfs_single_source(adj, source, {destination}, depth_limit);
}

// Backtracking graph search

// Wikipedia version
// This is an adaptation of the backtrack algorithm at:
// https://en.wikipedia.org/wiki/Backtracking#Pseudocode
// It replaces `first` and `next` with `adjacencies`.

// struct backtrack_graph {
//     using vertex_type = int;
//     using candidate_type = std::vector<vertex_type>;
//     candidate_type root() const;
//     bool reject(candidate_type& c) const;
//     bool accept(candidate_type& c) const;
//     auto adjacencies(const vertex_type& c) const;
//     void output(const candidate_type& c) const;
// };

// FIXME we can't actually assume that candidate_type has a
// `push_back`/`pop_back` interface - it might actually be something more like
// `make_move`/`undo_move`.

template <typename BacktrackGraph>
void backtrack(const BacktrackGraph& graph,
               typename BacktrackGraph::candidate_type& candidate)
{
    if (graph.reject(candidate))
        return;
    if (graph.accept(candidate))
        graph.output(candidate);
    for (auto v : graph.adjacencies(candidate.back())) {
        candidate.push_back(v);
        backtrack(graph, candidate);
        candidate.pop_back();
    }
}

template <typename BacktrackGraph>
void backtrack(const BacktrackGraph& graph)
{
    auto source{graph.root()};
    backtrack(graph, source);
}

template <typename BacktrackGraph>
struct range_stack_elem {
    using vertex_type = typename BacktrackGraph::vertex_type;
    range_stack_elem(const BacktrackGraph& g, const vertex_type& v)
        : range{g.adjacencies(v)}, iter{range.begin()}
    {
    }
    decltype(static_cast<BacktrackGraph*>(nullptr)->adjacencies({})) range;
    decltype(range.begin()) iter;
};

template <typename BacktrackGraph>
Generator<typename BacktrackGraph::candidate_type> backtrack_coro(
    BacktrackGraph& graph)
{
    auto candidate{graph.root()};

    std::vector<range_stack_elem<BacktrackGraph>> adjacencies_stack;
    adjacencies_stack.emplace_back(graph, candidate.back());

    while (!candidate.empty()) {
        auto iter = adjacencies_stack.back().iter;
        if (iter != adjacencies_stack.back().range.end()) {
            adjacencies_stack.back().iter++;
            candidate.push_back(*iter);
            adjacencies_stack.emplace_back(graph, *iter);

            if (graph.reject(candidate)) {
                candidate.pop_back();
                adjacencies_stack.pop_back();
                continue;
            }
            if (graph.accept(candidate)) {
                co_yield candidate;
                candidate.pop_back();
                adjacencies_stack.pop_back();
                continue;
            }
        }
        else {
            candidate.pop_back();
            adjacencies_stack.pop_back();
        }
    }
}

template <typename DijkstraGraph>
struct dijkstra_out {
    using vertex_type = typename DijkstraGraph::vertex_type;
    using cost_type = typename DijkstraGraph::cost_type;
    std::map<vertex_type, cost_type> dist;
    std::map<vertex_type, vertex_type> prev;
    std::optional<vertex_type> end;
};

template <typename DijkstraGraph>
dijkstra_out<DijkstraGraph> dijkstra(const DijkstraGraph& graph)
{
    using queue_entry = typename DijkstraGraph::queue_entry;
    using vertex_type = typename DijkstraGraph::vertex_type;
    const vertex_type start = graph.root();

    dijkstra_out<DijkstraGraph> out;
    out.dist.emplace(start, 0);

    using queue_t = std::priority_queue<queue_entry, std::vector<queue_entry>,
                                        std::greater<queue_entry>>;
    queue_t q;
    q.push({start, 0});

    while (!q.empty()) {
        const queue_entry e{q.top()};
        q.pop();
        const auto u{e.vert};  // best vertex
        if (graph.accept(u)) {
            out.end = u;
            return out;
        }

        const auto neighbors{graph.adjacencies(u)};
        for (auto neighbor : neighbors) {
            const auto& v{neighbor.vert};
            const auto alt{e.dist + neighbor.dist};
            if (!out.dist.contains(v) || alt < out.dist.at(v)) {
                out.dist.insert_or_assign(v, alt);
                out.prev.insert_or_assign(v, u);
                q.push({v, alt});
            }
        }
    }

    return out;
}

}  // namespace aoc

#endif  // AOC_GRAPH_HPP
