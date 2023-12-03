//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

using grid_t = dynamic_grid<char>;
using pos_t = vec2<int>;
using rect_t = rect<int>;

grid_t parse_grid(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    grid_t grid{static_cast<int>(lines[0].size()),
                static_cast<int>(lines.size())};
    const auto tiles{lines | rv::join |
                     rv::transform([](char c) { return c; })};
    r::copy(tiles, grid.data().data());
    return grid;
}

struct part {
    int number;
    rect_t position;

    rect_t border() const noexcept
    {
        return {position.base - pos_t{1, 1}, position.dimensions + pos_t{2, 2}};
    }
    auto valid_border_points(const grid_t& schematic) const noexcept
    {
        const auto is_valid_border_point{[this, &schematic](pos_t point) {
            return schematic.area().contains(point) &&
                   !position.contains(point);
        }};

        return border().all_points() | rv::filter(is_valid_border_point);
    }

    bool check_for_symbols(const grid_t& schematic) const noexcept
    {
        return r::any_of(
            valid_border_points(schematic) |
                rv::transform([&](pos_t point) { return schematic[point]; }),
            [](char c) { return c != '.'; });
    }
};

std::vector<part> find_parts(const grid_t& schematic)
{
    std::vector<part> parts;

    for (int row{0}; row < schematic.height(); row++) {
        int col{0};
        while (col < schematic.width()) {
            if (is_digit(schematic[{col, row}])) {
                int part_number{};
                rect_t part_pos{{col, row}, {0, 1}};
                while (col < schematic.width() &&
                       is_digit(schematic[{col, row}])) {
                    part_number *= 10;
                    part_number += schematic[{col, row}] - '0';
                    col++;
                    part_pos.dimensions.x++;
                }
                parts.push_back({part_number, {part_pos}});
            }
            else {
                col++;
            }
        }
    }

    return parts;
}

}  // namespace

aoc::solution_result day03(std::string_view input)
{
    grid_t schematic{parse_grid(input)};
    // TODO: General code for visualizing grids

    std::vector<part> parts{find_parts(schematic)};

    const auto is_part_number{
        [&](const part& p) { return p.check_for_symbols(schematic); }};

    const int part1{
        r::accumulate(parts | rv::filter(is_part_number) |
                          rv::transform([](const part& p) { return p.number; }),
                      0)};

    auto gear_positions{
        schematic.area().all_points() |
        rv::filter([&](pos_t p) { return schematic[p] == '*'; })};

    std::map<pos_t, std::set<int>> part_numbers_by_gear_pos;
    for (const part& p : parts) {
        for (pos_t gear : gear_positions) {
            if (r::contains(p.valid_border_points(schematic), gear)) {
                part_numbers_by_gear_pos[gear].insert(p.number);
            }
        }
    }

    int part2{0};
    for (const auto& [gear, part_numbers] : part_numbers_by_gear_pos) {
        if (part_numbers.size() == 2) {
            int ratio{1};
            for (int num : part_numbers) {
                ratio *= num;
            }
            part2 += ratio;
        }
    }

    return {part1, part2};
}

}  // namespace aoc::year2023
