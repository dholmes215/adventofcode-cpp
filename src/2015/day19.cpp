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

    const std::string start{"e"};
    const std::string destination{input_molecule};

    const auto adj_func{[&](const std::string& s) {
        return generate_replacement_molecules(replacements, s);
    }};

    const auto path{bfs_path(adj_func, start, destination)};

    return {part1_result, path.size()};
}

}  // namespace aoc::year2015
