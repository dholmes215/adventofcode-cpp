//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <fmt/ranges.h>

#include <map>
#include <set>
#include <string_view>

namespace aoc::year2022 {

namespace {

using pos_t = vec2<int>;
using rect_t = rect<int>;

rect_t find_bounds(const std::set<pos_t>& elves)
{
    pos_t min{*elves.begin()};
    pos_t max{min};
    for (const auto& elf : elves) {
        min = {std::min(min.x, elf.x), std::min(min.y, elf.y)};
        max = {std::max(max.x, elf.x), std::max(max.y, elf.y)};
    }
    return rect_from_corners(min, max);
}

// void print_elves(const std::set<pos_t>& elves, int round)
// {
//     fmt::print("== {} ==\n", round);
//     const auto bounds{find_bounds(elves)};
//     for (int y{bounds.base.y}; y < bounds.dimensions.y + bounds.base.y; y++)
//     {
//         fmt::print("{:2} ", y);
//         for (int x{bounds.base.x}; x < bounds.dimensions.x + bounds.base.x;
//              x++) {
//             fmt::print("{}", elves.contains({x, y}) ? '#' : '.');
//         }
//         fmt::print("\n");
//     }
//     fmt::print("\n");
// }

const std::array<std::pair<pos_t, std::array<pos_t, 3>>, 4> dirs{{
    {{0, -1}, {{{-1, -1}, {0, -1}, {1, -1}}}},
    {{0, 1}, {{{-1, 1}, {0, 1}, {1, 1}}}},
    {{-1, 0}, {{{-1, -1}, {-1, 0}, {-1, 1}}}},
    {{1, 0}, {{{1, -1}, {1, 0}, {1, 1}}}},
}};

const std::array<pos_t, 8> neighbors{
    {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

}  // namespace

aoc::solution_result day23(std::string_view input)
{
    std::set<pos_t> elves;
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    const int width{static_cast<int>(lines[0].size())};
    const int height{static_cast<int>(lines.size())};
    for (int y{0}; y < width; y++) {
        for (int x{0}; x < height; x++) {
            if (lines[static_cast<std::size_t>(y)]
                     [static_cast<std::size_t>(x)] == '#') {
                elves.insert({x, y});
            }
        }
    }

    auto dirs_copy{dirs};

    const auto propose{[&](pos_t elf) {
        if (r::none_of(neighbors,
                       [&](pos_t n) { return elves.contains(elf + n); })) {
            return elf;
        }

        const auto valid{[&](pos_t dir) { return elves.contains(elf + dir); }};
        for (const auto& [step, checks] : dirs_copy) {
            if (r::none_of(checks, valid)) {
                return elf + step;
            }
        }
        return elf;
    }};

    const auto do_round{[&] {
        std::map<pos_t, std::set<pos_t>> elves_by_proposal;
        for (const auto& elf : elves) {
            auto proposal{propose(elf)};
            if (proposal != elf) {
                elves_by_proposal[proposal].insert(elf);
            }
        }

        for (const auto& [proposal, proposers] : elves_by_proposal) {
            if (proposers.size() == 1) {
                elves.erase(*proposers.begin());
                elves.insert(proposal);
            }
        }
        r::rotate(dirs_copy, r::next(dirs_copy.begin()));
        return !elves_by_proposal.empty();
    }};

    for (int round{1}; round <= 10; round++) {
        do_round();
    }

    auto bounds{find_bounds(elves)};
    int part1{bounds.dimensions.x * bounds.dimensions.y -
              static_cast<int>(elves.size())};

    int round{11};
    while (do_round()) {
        round++;
    }

    return {part1, round};
}

}  // namespace aoc::year2022
