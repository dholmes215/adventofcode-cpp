//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_font.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <fmt/format.h>
#include <fmt/xchar.h>

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

// void print_grid(const auto& grid)
// {
//     for (const auto row : grid.rows()) {
//         for (dot_t d : row) {
//             fmt::print("{}", d);
//         }
//         fmt::print("\n");
//     }
// }

}  // namespace

std::uint8_t pack_bools(std::span<const bool, 8> bools) {
    std::uint8_t result{0};
    for (std::size_t i{0}; i < 8; ++i) {
        if (bools[i]) {
            result |= 1 << i;
        }
    }
    return result;
}

// wchar_t to_braille(auto&& row1, auto&& row2, auto&& row3, auto&& row4)
// {
//     std::array<bool, 8> bools{0};
//     r::copy(row1, bools.begin());
//     r::copy(row2, bools.begin()+2);
//     r::copy(row3, bools.begin()+4);
//     r::copy(row4, bools.begin()+6);
//     std::swap(bools[0], bools[0]);
//     std::swap(bools[1], bools[5]);
//     std::swap(bools[2], bools[3]);
//     std::swap(bools[3], bools[4]);
//     std::swap(bools[4], bools[2]);
//     std::swap(bools[5], bools[5]);
//     std::swap(bools[6], bools[6]);
//     std::swap(bools[7], bools[7]);
//     constexpr braile_start{0x2800};
//     return braile_start + pack_bools(bools);
// }

wchar_t to_braille(std::span<const bool, 4> bit_col_1, std::span<const bool, 4> bit_col_2)
{
    std::array<bool, 8> bools{0};
    bools[0] = bit_col_1[0];
    bools[1] = bit_col_1[1];
    bools[2] = bit_col_1[2];
    bools[3] = bit_col_2[0];
    bools[4] = bit_col_2[1];
    bools[5] = bit_col_2[2];
    bools[6] = bit_col_1[3];
    bools[7] = bit_col_2[3];
    constexpr wchar_t braile_start{0x2800};
    return braile_start + pack_bools(bools);
}

std::array<bool, 4> to_bit_col(bool a, bool b, bool c, bool d) {
    return {a, b, c, d};
}

void print_grid(const auto& grid)
{
    for (const auto four_rows : grid.rows() | rv::chunk(4)) {
        auto row_iter = four_rows.begin();
        const auto row1 = *row_iter++;
        const auto row2 = *row_iter++;
        const auto row3 = *row_iter++;
        const auto row4 = *row_iter++;
        const auto cols{rv::zip_with(to_bit_col, row1, row2, row3, row4) | r::to<std::vector>};
        const auto chunks{ cols | rv::chunk(2) | r::to<std::vector> };
        const auto brailles{chunks | rv::transform([](auto&& cols2) {
            auto col_iter = cols2.begin();
            const auto col1 = *col_iter;
            col_iter++;
            const auto col2 = *col_iter;
            col_iter++;
            return to_braille(col1, col2);
        })};
        

        for (wchar_t wc : brailles) {
            fmt::print("{}", wc);
        }
        fmt::print("\n");
    }
}

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

        // if (current_area.dimensions.x < 200 && current_area.dimensions.y < 200) {
        // fmt::print("After fold: \n");
        //     print_grid(grid.subgrid(current_area));
        // }
        if (!dots_after_first_fold) {
            dots_after_first_fold =
                r::count(grid.subgrid(current_area).data(), '#');
        }
    }

    std::string activation_code;
    for (int i{0}; i < 8; i++) {
        fmt::print("---------\n");
        print_grid(grid.subgrid({{i * 5, 0}, {4, 6}}));

        std::array<dot_t, 24> char_array;
        r::copy(grid.subgrid({{i * 5, 0}, {4, 6}}).data(), char_array.begin());
        activation_code += recognize_char(char_array);
    }

    return {*dots_after_first_fold, activation_code};
}

}  // namespace aoc::year2021
