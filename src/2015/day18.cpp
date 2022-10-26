//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <gsl/narrow>

#include <string_view>

namespace aoc::year2015 {

namespace {

using light_t = char;
using light_grid_t = dynamic_grid<light_t>;
using point_t = vec2<int>;

light_grid_t parse_grid(std::string_view input)
{
    const auto lines{sv_lines(trim(input))};
    const auto width{r::front(lines).size()};
    const auto height{r::distance(sv_lines(input))};

    light_grid_t out{gsl::narrow_cast<int>(width),
                     gsl::narrow_cast<int>(height)};

    r::copy(lines | rv::join, out.data().begin());

    return out;
}

constexpr std::array<point_t, 8> directions{
    {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}}};

auto get_neighbor_points(const rect<int>& r, point_t p) noexcept
{
    return directions | rv::transform([p](auto p2) { return p + p2; }) |
           rv::filter([&](point_t p2) { return r.contains(p2); });
}

light_t next_light_value(const light_grid_t& lights, point_t point)
{
    char current{lights[point]};
    // XXX If I don't store these in a vector, there's a lifetime error with the
    // next two lines and I need to understand it
    auto neighbor_points{get_neighbor_points(lights.area(), point) |
                         r::to<std::vector>};
    auto neighbor_values{neighbor_points |
                         rv::transform([&](point_t p) { return lights[p]; })};
    const auto on_count{r::count(neighbor_values, '#')};

    if (current == '#' && (on_count == 2 || on_count == 3)) {
        return '#';
    }
    else if (current == '.' && on_count == 3) {
        return '#';
    }
    return '.';
}

void generate_next_grid1(const light_grid_t& current, light_grid_t& next_out)
{
    for (point_t p : current.area().all_points()) {
        next_out[p] = next_light_value(current, p);
    }
}

void generate_next_grid2(const light_grid_t& current, light_grid_t& next_out)
{
    generate_next_grid1(current, next_out);
    next_out[{0, 0}] = '#';
    next_out[{next_out.width() - 1, 0}] = '#';
    next_out[{0, next_out.height() - 1}] = '#';
    next_out[{next_out.width() - 1, next_out.height() - 1}] = '#';
}

}  // namespace

aoc::solution_result day18(std::string_view input)
{
    const light_grid_t initial_lights{parse_grid(input)};
    light_grid_t lights{initial_lights};
    light_grid_t next{lights.width(), lights.height()};

    for (int i{0}; i < 100; i++) {
        generate_next_grid1(lights, next);
        std::swap(lights, next);
    }

    const auto light_count1{r::count(lights.data(), '#')};

    lights = initial_lights;
    for (int i{0}; i < 100; i++) {
        generate_next_grid2(lights, next);
        std::swap(lights, next);
    }

    const auto light_count2{r::count(lights.data(), '#')};

    return {light_count1, light_count2};
}

}  // namespace aoc::year2015
