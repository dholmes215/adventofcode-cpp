//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

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

auto calibrate_machine(const replacement_map_t& replacements,
                       const std::string_view input_molecule)
{
    const auto atom_positions{split_molecule(input_molecule)};
    std::set<std::string> new_molecules;

    for (const auto& [pos, count] : atom_positions) {
        const std::string_view input_atom{input_molecule.substr(pos, count)};
        const auto [begin, end]{replacements.equal_range(input_atom)};
        const auto replacement_values{r::subrange{begin, end} | rv::values};
        for (const auto replacement : replacement_values) {
            std::string new_molecule{input_molecule.begin(),
                                     input_molecule.end()};
            new_molecule.replace(pos, count, replacement);
            new_molecules.insert(std::move(new_molecule));
        }
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

    return {part1_result, ""};
}

}  // namespace aoc::year2015
