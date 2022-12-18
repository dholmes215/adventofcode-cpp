//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <array>
#include <map>
#include <queue>
#include <set>
#include <string_view>

namespace aoc::year2022 {

namespace {

using int_t = int;
using coord_t = vec3<int_t>;
const coord_t origin{0, 0, 0};

coord_t parse_line(std::string_view line)
{
    auto nums{numbers<int_t>(line) | r::to<std::vector>};
    return {nums[0], nums[1], nums[2]};
}

std::array<coord_t, 6> directions{coord_t{-1, 0, 0}, coord_t{1, 0, 0},
                                  coord_t{0, -1, 0}, coord_t{0, 1, 0},
                                  coord_t{0, 0, -1}, coord_t{0, 0, 1}};

auto neighbors(const coord_t& c)
{
    return directions | rv::transform([c](const coord_t d) { return c + d; });
}

coord_t max(const coord_t& a, const coord_t& b)
{
    return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)};
}

bool in_cuboid(const coord_t& c, const coord_t& dim)
{
    return c.x >= 0 && c.y >= 0 && c.z >= 0 && c.x < dim.x && c.y < dim.y &&
           c.z < dim.z;
}

std::set<coord_t> bfs_water_cubes_from_origin(const coord_t& dimensions,
                                              const std::set<coord_t>& lava)
{
    std::set<coord_t> out{{origin}};
    enum class graph_color { white, gray, black };
    using queue = std::queue<coord_t>;
    std::map<coord_t, graph_color> colors;

    colors[origin] = graph_color::gray;

    queue q;
    q.push(origin);

    const auto adj{[&](const coord_t& c) {
        return neighbors(c) | rv::filter([&](const coord_t& n) {
                   return in_cuboid(n, dimensions) && !lava.contains(n);
               });
    }};

    while (!q.empty()) {
        coord_t u{q.front()};
        q.pop();
        out.insert(u);
        for (const coord_t& v : adj(u)) {
            auto& v_color{colors[v]};
            if (v_color == graph_color::white) {
                v_color = graph_color::gray;
                q.push(v);
            }
        }
        colors[u] = graph_color::black;
    }

    return out;
}

}  // namespace

aoc::solution_result day18(std::string_view input)
{
    const auto cubes{sv_lines(trim(input)) | rv::transform(parse_line) |
                     r::to<std::set>};
    const auto is_in_set{[&](const coord_t c) { return cubes.contains(c); }};
    const auto count_free_sides{[&](const coord_t c) {
        return 6 - r::count_if(neighbors(c), is_in_set);
    }};
    const auto surface_area{
        r::accumulate(cubes | rv::transform(count_free_sides), 0)};

    // identify enclosing cuboid
    const coord_t dimensions{r::accumulate(cubes, origin, max) +
                             coord_t{2, 2, 2}};

    const auto water{bfs_water_cubes_from_origin(dimensions, cubes)};
    std::set<coord_t> droplet_and_interior{cubes};
    fmt::print("{} {} {}\n", dimensions.x, dimensions.y, dimensions.z);
    for (int x : rv::iota(0, dimensions.x)) {
        for (int y : rv::iota(0, dimensions.y)) {
            for (int z : rv::iota(0, dimensions.z)) {
                coord_t c{x, y, z};
                if (!water.contains(c)) {
                    droplet_and_interior.insert(c);
                }
            }
        }
    }

    const auto is_in_set2{
        [&](const coord_t c) { return droplet_and_interior.contains(c); }};
    const auto count_free_sides2{[&](const coord_t c) {
        return 6 - r::count_if(neighbors(c), is_in_set2);
    }};
    const auto surface_area2{r::accumulate(
        droplet_and_interior | rv::transform(count_free_sides2), 0)};

    return {surface_area, surface_area2};
}

}  // namespace aoc::year2022
