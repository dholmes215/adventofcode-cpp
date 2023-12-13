//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <coro_generator.hpp>

#include <fmt/ranges.h>

#include <algorithm>
#include <set>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

using grid_t = dynamic_grid<char>;
using pos_t = vec2<int>;

grid_t parse_grid(std::vector<std::string_view> lines)
{
    // TODO: Make this function general and quit copy/pasting it everywhere
    grid_t grid{static_cast<int>(lines[0].size()),
                static_cast<int>(lines.size())};
    const auto tiles{lines | rv::join |
                     rv::transform([](char c) { return c; })};
    r::copy(tiles, grid.data().data());
    return grid;
}

std::vector<grid_t> parse_grids(std::string_view input)
{
    return sv_lines(trim(input)) | rv::split("") |
           rv::transform([](auto&& rng) {
               return parse_grid(rng | r::to<std::vector>);
           }) |
           r::to<std::vector>;
}

auto is_mirror{[](auto&& line) {
    auto middle{r::distance(line) / 2};
    return r::equal(line | rv::take(middle),
                    line | rv::reverse | rv::take(middle));
}};

Generator<int> find_reflections(auto lines)
{
    int length{static_cast<int>(r::distance(lines[0]))};
    for (int split{1}; split < length; split++) {
        int after{length - split};
        int start{0};
        int end{length};
        if (after > split) {
            end = split * 2;
        }
        else {
            start = split - after;
        }
        int width{end - start};
        auto adjusted_rows{lines | rv::transform([&](auto&& line) {
                               return line | rv::drop(start) | rv::take(width);
                           })};

        if (r::all_of(adjusted_rows, is_mirror)) {
            co_yield split;
        }
    }
}

}  // namespace

aoc::solution_result day13(std::string_view input)
{
    auto grids{parse_grids(trim(input))};

    int part1{0};
    for (const grid_t& grid : grids) {
        for (int i : find_reflections(grid.rows())) {
            part1 += i;
        }
        for (int i : find_reflections(grid.cols())) {
            part1 += i * 100;
        }
    }

    int part2{0};
    for (const grid_t& grid : grids) {
        std::vector<int> original_reflections;
        for (int i : find_reflections(grid.rows())) {
            original_reflections.push_back(i);
        }
        for (int i : find_reflections(grid.cols())) {
            original_reflections.push_back(i * 100);
        }

        auto copy{grid};
            std::set<int> new_reflections;
        for (pos_t pos : copy.area().all_points()) {
            char& c{copy[pos]};
            c = (c == '.') ? '#' : '.';
            for (int i : find_reflections(copy.rows())) {
                new_reflections.insert(i);
            }
            for (int i : find_reflections(copy.cols())) {
                new_reflections.insert(i * 100);
            }
            c = (c == '.') ? '#' : '.';
        }
        for (int i : original_reflections) {
            new_reflections.erase(i);
        }
        part2 += r::accumulate(new_reflections, 0);
    }

    return {part1, part2};
}

}  // namespace aoc::year2023
