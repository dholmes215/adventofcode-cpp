//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_braille.hpp>
#include <aoc_font.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <fmt/format.h>

#include <optional>
#include <string_view>

namespace aoc::year2021 {

namespace {

using int_t = int;
using coord_t = vec2<int_t>;
using rect_t = rect<int_t>;
using dot_t = char;
constexpr int_t grid_size{2048};
using grid_t = heap_grid<dot_t, grid_size, grid_size>;

struct fold_t {
    char axis;
    int_t pos;
};

fold_t to_fold(std::string_view s)
{
    const std::string_view last{s.substr(s.find_last_of('=') - 1)};
    return {last[0], to_num<int_t>(last.substr(2))};
}

// bool dot_t_to_bool(dot_t c)
// {
//     return c == '#';
// }

// void print_grid(const auto& grid)
// {
//     return print_grid_braille(grid, dot_t_to_bool);
// }

}  // namespace

aoc::solution_result day13(std::string_view input)
{
    const auto divide{input.find('f')};
    const auto coords{
        numbers<int_t>(input.substr(0, divide)) | rv::chunk(2) |
        rv::transform([](auto&& x) {
            return coord_t{r::front(x), r::front(x | rv::drop(1))};
        }) |
        r::to<std::vector>};

    rect_t current_area{
        coord_t{0, 0},
        coord_t{1, 1} +
            r::accumulate(coords, coord_t{0, 0}, [](coord_t lhs, coord_t rhs) {
                return coord_t{std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y)};
            })};

    grid_t grid;
    r::fill(grid.data(), '.');
    for (const auto& c : coords) {
        grid[c] = '#';
    }

    const auto folds{sv_lines(input.substr(divide)) | rv::transform(to_fold) |
                     r::to<std::vector>};

    std::optional<std::size_t> dots_after_first_fold{};
    for (const auto& fold : folds) {
        if (fold.axis == 'x') {
            auto new_width{fold.pos};
            current_area = {{0, 0}, {new_width, current_area.dimensions.x}};
            for (auto p : current_area.all_points()) {
                coord_t flip_point{new_width * 2 - p.x, p.y};
                if (grid[flip_point] == '#') {
                    grid[p] = '#';
                }
            }
        }
        else {
            auto new_height{fold.pos};
            current_area = {{0, 0}, {current_area.dimensions.x, new_height}};
            for (auto p : current_area.all_points()) {
                coord_t flip_point{p.x, new_height * 2 - p.y};
                if (grid[flip_point] == '#') {
                    grid[p] = '#';
                }
            }
        }

        // if (current_area.dimensions.x < 400 &&
        //     current_area.dimensions.y < 400) {
        //     fmt::print("After fold: \n");
        //     print_grid(grid.subgrid(current_area));
        // }
        if (!dots_after_first_fold) {
            dots_after_first_fold =
                r::count(grid.subgrid(current_area).data(), '#');
        }
    }

    std::string activation_code;
    for (int i{0}; i < 8; i++) {
        std::array<dot_t, 30> char_array;
        r::copy(grid.subgrid({{i * 5, 0}, {5, 6}}).data(), char_array.begin());
        activation_code += recognize_char(char_array);
    }

    return {*dots_after_first_fold, activation_code};
}

}  // namespace aoc::year2021
