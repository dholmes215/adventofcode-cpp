//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_braille.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <string_view>

namespace aoc::year2022 {

namespace {

using point_t = vec2<int>;
using rect_t = rect<int>;
using grid_t = dynamic_grid<char>;

std::vector<point_t> parse_line(std::string_view line)
{
    return numbers<int>(line) | rv::chunk(2) | rv::transform([](auto rng) {
               auto iter{rng.begin()};
               const auto x{*iter++};
               const auto y{*iter};
               return point_t{x, y};
           }) |
           r::to<std::vector>;
}

constexpr point_t source{500, 0};

point_t bottom_right(point_t p1, point_t p2)
{
    return {1000, std::max(p1.y, p2.y)};
}

point_t find_bottom_right(const auto& points)
{
    return r::accumulate(points, source, bottom_right) + point_t{1, 3};
}

grid_t parse_input(std::string_view input)
{
    const auto structures{sv_lines(trim(input)) | rv::transform(parse_line) |
                          r::to<std::vector>};
    const auto [width, height]{find_bottom_right(structures | rv::join)};
    grid_t out{width, height};
    r::fill(out.data(), '.');
    r::fill(out.row(out.height() - 1), '#');
    const auto structure_line_rects{[](const std::vector<point_t>& structure) {
        return structure | rv::sliding(2) | rv::transform([](auto rng) {
                   auto iter{rng.begin()};
                   const auto a{*iter++};
                   const auto b{*iter};
                   return rect_from_corners(a, b);
               });
    }};

    const auto rect_to_points{[](rect_t rect) { return rect.all_points(); }};

    const auto all_rects{structures | rv::transform(structure_line_rects) |
                         rv::join | r::to<std::vector>};
    const auto all_structure_points{all_rects | rv::transform(rect_to_points) |
                                    rv::join | r::to<std::vector>};
    for (const auto& p : all_structure_points) {
        out[p] = '#';
    }
    return out;
}

void drop_sand(grid_t& grid)
{
    point_t p{source};
    while (true) {
        if (grid[p + point_t{0, 1}] == '.') {
            p += point_t{0, 1};
        }
        else if (grid[p + point_t{-1, 1}] == '.') {
            p += point_t{-1, 1};
        }
        else if (grid[p + point_t{1, 1}] == '.') {
            p += point_t{1, 1};
        }
        else {
            break;
        }
    }
    grid[p] = 'o';
}

// void print_grid(const auto& grid)
// {
//     // for (const auto row : grid.rows()) {
//     //     auto str{row | r::to<std::string>};
//     //     fmt::print("{}\n", str);
//     // }
//     print_grid_braille(grid, [](char c) { return c != '.'; });
// }

}  // namespace

aoc::solution_result day14(std::string_view input)
{
    grid_t grid{parse_input(input)};
    auto bottom_row{grid.row(grid.height() - 2)};

    auto done1{[&] {
        return !r::all_of(bottom_row, [](char c) { return c == '.'; });
    }};
    auto done2{[&] { return grid[source] == 'o'; }};
    auto count_sand{[&] { return r::count(grid.data(), 'o'); }};

    // auto shown_subgrid{grid.subgrid(
    //     rect_from_corners(point_t{325, 0}, point_t{675, grid.height() -
    //     1}))};
    // print_grid(shown_subgrid);

    while (!done1()) {
        drop_sand(grid);
    }
    const auto part1_sand{count_sand() - 1};
    // print_grid(shown_subgrid);

    while (!done2()) {
        drop_sand(grid);
    }
    const auto part2_sand{count_sand()};
    // print_grid(shown_subgrid);

    return {part1_sand, part2_sand};
}

}  // namespace aoc::year2022
