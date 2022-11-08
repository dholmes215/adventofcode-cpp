//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_graph.hpp>
#include <aoc_range.hpp>
#include <coro_generator.hpp>

#include <ctre.hpp>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <map>
#include <set>
#include <string>
#include <string_view>

namespace aoc::year2015 {
namespace {

using replacement_map_t = std::multimap<std::string_view, std::string_view>;
using replacement_t = std::pair<std::string_view, std::string_view>;

replacement_t parse_replacement(std::string_view line)
{
    constexpr auto matcher{ctre::match<R"((.+) => (.+))">};
    if (auto [whole, before, after] = matcher(line); whole) {
        return {before.to_view(), after.to_view()};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

template <typename Key, typename Value>
[[nodiscard]] std::multimap<Value, Key> invert_multimap(
    const std::multimap<Key, Value>& input)
{
    const auto flip_pair{[](std::pair<Key, Value> p) {
        return std::pair<Value, Key>{p.second, p.first};
    }};
    return input | rv::transform(flip_pair) | r::to<std::multimap>;
}

struct atom_pos_t {
    std::size_t pos;
    std::size_t count;
};

auto split_molecule(std::string_view molecule)
{
    // Nasty special-case hack for "e", which the code below doesn't support
    // because it doesn't start with an upper-case letter.
    if (molecule == "e") {
        return std::vector<atom_pos_t>{{0, 1}};
    }

    const auto second_isupper{
        [](auto pair) -> bool { return std::isupper(pair.second); }};
    const auto offset_pair_to_pos{[](auto pair) {
        return atom_pos_t{pair[0], pair[1] - pair[0]};
    }};

    const auto offsets{rv::concat(molecule | rv::enumerate |
                                      rv::filter(second_isupper) | rv::keys,
                                  rv::single(molecule.size())) |
                       r::to<std::vector>};
    return offsets | rv::sliding(2) | rv::transform(offset_pair_to_pos) |
           r::to<std::vector>;
}

Generator<std::string> generate_replacement_molecules(
    const replacement_map_t& replacements,
    const std::string_view input_molecule)
{
    const auto atom_positions{split_molecule(input_molecule)};

    for (const auto& [pos, count] : atom_positions) {
        const std::string_view input_atom{input_molecule.substr(pos, count)};
        const auto replacement_values{
            multimap_value_range(replacements, input_atom)};
        for (const auto replacement : replacement_values) {
            std::string new_molecule{input_molecule.begin(),
                                     input_molecule.end()};
            new_molecule.replace(pos, count, replacement);
            co_yield new_molecule;
        }
    }
}

// Iterates all the molecules that could have produced the given molecule
Generator<std::string> generate_originating_molecules(
    const replacement_map_t& inverted_replacements,
    const std::string_view output_molecule)
{
    const auto keys{inverted_replacements | rv::keys | rv::unique};
    for (const auto& key : keys) {
        auto pos{output_molecule.find(key, 0)};
        while (pos != std::string_view::npos) {
            for (const auto& value :
                 multimap_value_range(inverted_replacements, key)) {
                std::string orig_molecule{output_molecule};
                orig_molecule.replace(pos, key.size(), value);
                if (value == "e" && orig_molecule != "e") {
                    // Only dead ends this way.
                    continue;
                }
                co_yield orig_molecule;
            }

            pos = output_molecule.find(key, pos + 1);
        }
    }
}

auto calibrate_machine(const replacement_map_t& replacements,
                       const std::string_view input_molecule)
{
    std::set<std::string> new_molecules;

    for (auto new_molecule :
         generate_replacement_molecules(replacements, input_molecule)) {
        new_molecules.insert(std::move(new_molecule));
    }

    return new_molecules.size();
}

}  // namespace

aoc::solution_result day19(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    const replacement_map_t replacements{lines | rv::take(lines.size() - 2) |
                                         rv::transform(parse_replacement) |
                                         r::to<std::multimap>};
    const auto input_molecule{lines.back()};

    const auto part1_result{calibrate_machine(replacements, input_molecule)};

    // Part 2: BFS? A* with a heuristic based on length of molecule?

    const auto inverted_replacements{invert_multimap(replacements)};

    const std::string start{input_molecule};
    const std::string destination{"e"};

    const auto adj_func{[&](const std::string& s) {
        return generate_originating_molecules(inverted_replacements, s);
    }};

    // const auto path{bfs_path(adj_func, start, destination)};
    const auto path{dfs_path(adj_func, start, destination)};

    // Plain BFS doesn't work.  Try better pruning?  A*? Greedy BFS with
    // Levenshtein distance heuristic or something? Greedy BFS with just string
    // length heuristic? IDDFS/IDA*?

    // Try ditching the distance numbers?  Try ditching the path?

    return {part1_result, path.size() - 1};
}

}  // namespace aoc::year2015
