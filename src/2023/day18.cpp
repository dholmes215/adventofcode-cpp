//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_braille.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>

#include <ctre.hpp>

#include <fmt/ranges.h>

#include <algorithm>
#include <set>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

using int_t = std::int32_t;
using big_int_t = std::int64_t;
using grid_t = dynamic_grid<char>;
using pos_t = vec2<int>;

struct rgb {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

rgb parse_color(std::string_view sv)
{
    return {to_num_base<std::uint8_t>(sv.substr(0, 2), 16),
            to_num_base<std::uint8_t>(sv.substr(2, 2), 16),
            to_num_base<std::uint8_t>(sv.substr(4, 2), 16)};
}

struct trench {
    pos_t direction;
    int_t meters;
};

constexpr const pos_t up{0, -1};
constexpr const pos_t down{0, 1};
constexpr const pos_t left{-1, 0};
constexpr const pos_t right{1, 0};

constexpr const std::array<pos_t, 4> cardinal_directions{
    {up, down, left, right}};

pos_t letter_to_dir(char c)
{
    switch (c) {
        case 'U':
            return up;
        case 'D':
            return down;
        case 'L':
            return left;
        case 'R':
            return right;
    }
    throw input_error{fmt::format("unsupported direction: {}", c)};
}

trench part1_parse_trench(std::string_view line)
{
    constexpr auto matcher{ctre::match<R"((.) (.+) \(#.{6}\))">};
    if (auto [whole, direction, meters] = matcher(line); whole) {
        return {letter_to_dir(direction.to_view()[0]), to_num<int_t>(meters)};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

pos_t digit_to_dir(char c)
{
    switch (c) {
        case '0':
            return right;
        case '1':
            return down;
        case '2':
            return left;
        case '3':
            return up;
    }
    throw input_error{fmt::format("unsupported direction: {}", c)};
}

trench part2_parse_trench(std::string_view line)
{
    constexpr auto matcher{ctre::match<R"(. .+ \(#(.{6})\))">};
    if (auto [whole, rgb] = matcher(line); whole) {
        std::string_view hex_string{rgb};
        return {digit_to_dir(hex_string[5]),
                to_num_base<int_t>(hex_string.substr(0, 5), 16)};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

pos_t right_hand_direction(pos_t p)
{
    if (p == up) {
        return right;
    }
    if (p == down) {
        return left;
    }
    if (p == left) {
        return up;
    }
    if (p == right) {
        return down;
    }
    throw input_error(fmt::format("Invalid direction: {},{}", p.x, p.y));
}

void flood_fill(const grid_t& grid, pos_t pos, char c)
{
    std::set<pos_t> to_fill{pos};
    while (!to_fill.empty()) {
        pos_t filling{r::front(to_fill)};
        auto neighbors{cardinal_directions |
                       rv::transform([&](pos_t p2) { return p2 + filling; }) |
                       rv::filter([&](pos_t p2) {
                           return grid.area().contains(p2) && grid[p2] == '.';
                       })};
        for (pos_t neighbor : neighbors) {
            to_fill.insert(neighbor);
        }
        grid[filling] = c;
        to_fill.erase(filling);
    }
}

struct start_parameters {
    pos_t start;
    pos_t dimensions;
};

start_parameters find_start_parameters(const std::vector<trench>& trenches)
{
    const pos_t start{0, 0};

    pos_t pos{0, 0};
    pos_t top_left{pos};
    pos_t bottom_right{pos};
    for (const trench& t : trenches) {
        pos += t.direction * t.meters;
        top_left = {std::min(top_left.x, pos.x), std::min(top_left.y, pos.y)};
        bottom_right = {std::max(bottom_right.x, pos.x),
                        std::max(bottom_right.y, pos.y)};
    }

    return {pos_t{0 - top_left.x, 0 - top_left.y},
            rect_from_corners(top_left, bottom_right).dimensions};
}

}  // namespace

void print_grid(const grid_t& grid)
{
    for (int row{0}; row < grid.height(); row++) {
        for (int col{0}; col < grid.width(); col++) {
            fmt::print("{}", grid[{col, row}]);
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

aoc::solution_result day18(std::string_view input)
{
    big_int_t part1{};
    {
        const std::vector<trench> part1_trenches{
            sv_lines(trim(input)) | rv::transform(part1_parse_trench) |
            r::to<std::vector>};

        // for (const trench& t : part1_trenches) {
        //     fmt::print("{},{} {} \x1B[38;2;{};{};{}m{},{},{}\x1B[0m\n",
        //                t.direction.x, t.direction.y, t.meters, t.color.r,
        //                t.color.g, t.color.b, t.color.r, t.color.g,
        //                t.color.b);
        // }

        const auto [start, dimensions]{find_start_parameters(part1_trenches)};

        grid_t grid{dimensions.x, dimensions.y};
        r::fill(grid.data(), '.');
        pos_t pos{start};
        grid[pos] = '#';

        for (const trench& t : part1_trenches) {
            for (int i{0}; i < t.meters; i++) {
                pos += t.direction;
                grid[pos] = '#';
            }
        }

        // print_grid(grid);
        // print_grid_braille(grid, [](char c) { return c != '.'; });

        // pos = start;
        // for (const trench& t : part1_trenches) {
        //     for (int i{0}; i < t.meters; i++) {
        //         pos += t.direction;
        //         grid[pos] = '#';
        //     }
        // }

        if (part1_trenches[0].direction != right) {
            throw input_error("First direction must be R");
        }

        for (const trench& t : part1_trenches) {
            for (int i{0}; i < t.meters; i++) {
                pos += t.direction;
                pos_t inside{pos + right_hand_direction(t.direction)};
                if (grid[inside] == '.') {
                    flood_fill(grid, inside, '#');
                }
            }
        }

        // print_grid_braille(grid, [](char c) { return c != '.'; });

        part1 = r::count(grid.data(), '#');
    }

    //////////////////////////////////////////////

    big_int_t part2{};
    {
        const std::vector<trench> part2_trenches{
            sv_lines(trim(input)) | rv::transform(part2_parse_trench) |
            r::to<std::vector>};

        const auto [start, dimensions]{find_start_parameters(part2_trenches)};
        fmt::print("Part 2 adjusted start: {},{}\n", start.x, start.y);

        std::vector<int_t> x_stopping_points{{start.x}};
        std::vector<int_t> y_stopping_points{{start.y}};

        {
            pos_t pos{start};
            for (const trench& t : part2_trenches) {
                pos += t.direction * t.meters;
                x_stopping_points.push_back(pos.x);
                y_stopping_points.push_back(pos.y);
            }
        }

        r::sort(x_stopping_points);
        x_stopping_points.erase(r::unique(x_stopping_points),
                                x_stopping_points.end());
        r::sort(y_stopping_points);
        y_stopping_points.erase(r::unique(y_stopping_points),
                                y_stopping_points.end());

        fmt::print("x: {}\n", x_stopping_points);
        fmt::print("y: {}\n", y_stopping_points);

        // Map coordinates in the original coordinate system to a compressed
        // grid.  The original coordinate will be used to look up the first key
        // that's less-than or equal, and the corresponding value will be the
        // new coordinate.
        const auto make_compression_map{
            [](const std::vector<int_t>& stopping_points) {
                std::vector<std::pair<int_t, int_t>> compression_map{};
                int_t compressed{0};
                for (int_t i : stopping_points) {
                    compression_map.push_back({i, compressed++});
                    compression_map.push_back({i + 1, compressed++});
                }
                compression_map.pop_back();
                return compression_map;
            }};

        const auto x_compression_map{make_compression_map(x_stopping_points)};
        const auto y_compression_map{make_compression_map(y_stopping_points)};

        fmt::print("x: {}\n", x_compression_map);
        fmt::print("y: {}\n", y_compression_map);

        const auto compress_pos{[&](pos_t pos) {
            auto x_keys{x_compression_map | rv::keys};
            auto y_keys{y_compression_map | rv::keys};
            auto x_key_iter{r::prev(r::upper_bound(x_keys, pos.x))};
            auto y_key_iter{r::prev(r::upper_bound(y_keys, pos.y))};
            auto x_dist{r::distance(x_keys.begin(), x_key_iter)};
            auto y_dist{r::distance(y_keys.begin(), y_key_iter)};
            return pos_t{x_compression_map[x_dist].second,
                         y_compression_map[y_dist].second};
        }};

        // Map indexes in the compressed grid to the length to which the
        // corresponded in the original coordinate system.
        const auto make_expanded_lengths{
            [](const std::vector<int_t>& stopping_points) {
                std::vector<int_t> differences(stopping_points.size());
                r::adjacent_difference(stopping_points, differences.begin());
                std::vector<int_t> expanded_lengths;
                expanded_lengths.reserve(differences.size() * 2 + 1);
                expanded_lengths.push_back(1);
                for (int_t diff : differences | rv::drop(1)) {
                    expanded_lengths.push_back(diff);
                    expanded_lengths.push_back(1);
                }
                return expanded_lengths;
            }};

        const auto x_expanded_lengths{make_expanded_lengths(x_stopping_points)};
        const auto y_expanded_lengths{make_expanded_lengths(y_stopping_points)};

        fmt::print("x: {}\n", x_expanded_lengths);
        fmt::print("y: {}\n", y_expanded_lengths);

        grid_t grid{static_cast<int>(x_expanded_lengths.size()),
                    static_cast<int>(y_expanded_lengths.size())};
        r::fill(grid.data(), '.');
        pos_t pos{start};
        grid[compress_pos(pos)] = '#';

        for (const trench& t : part2_trenches) {
            pos_t compressed{compress_pos(pos)};
            pos_t new_pos{pos + t.direction * t.meters};
            pos_t new_compressed(compress_pos(new_pos));

            while (compressed != new_compressed) {
                compressed += t.direction;
                grid[compressed] = '#';
            }

            pos = new_pos;
        }

        print_grid_braille(grid, [](char c) { return c != '.'; });

        for (const trench& t : part2_trenches) {
            pos_t compressed{compress_pos(pos)};
            pos_t new_pos{pos + t.direction * t.meters};
            pos_t new_compressed(compress_pos(new_pos));

            while (compressed != new_compressed) {
                compressed += t.direction;
                pos_t inside{compressed + right_hand_direction(t.direction)};
                if (grid[inside] == '.') {
                    flood_fill(grid, inside, '#');
                }
            }

            pos = new_pos;
        }
        
        print_grid_braille(grid, [](char c) { return c != '.'; });

        const auto expanded_area{[&](pos_t p) {
            big_int_t len_x{x_expanded_lengths[p.x]};
            big_int_t len_y{x_expanded_lengths[p.x]};
            return len_x * len_y;
        }};

        for (pos_t p : grid.area().all_points()) {
            if (grid[p] == '#') {
                part2 += expanded_area(p);
            }
        }
    }

    return {part1, part2};
}

}  // namespace aoc::year2023
