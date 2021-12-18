//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <ctre.hpp>

#include <algorithm>
#include <cstdint>
#include <string_view>

namespace aoc::year2021 {

namespace {

using int_t = std::int64_t;
using pos_t = vec2<int_t>;
using velocity_t = vec2<int_t>;
using target_area_t = rect<int_t>;
constexpr pos_t start_pos{0, 0};

struct drone_t {
    pos_t pos{start_pos};
    velocity_t vel{0, 0};

    drone_t& operator++() noexcept  // pre-increment
    {
        pos += vel;
        vel.x = std::max(vel.x - 1, int_t{0});
        vel.y--;
        return *this;
    }

    drone_t operator++(int) noexcept  // post-increment
    {
        const drone_t out{*this};
        ++*this;
        return out;
    }
};

bool has_failed(const drone_t& drone, const target_area_t& target_area) noexcept
{
    const auto target_x_end = target_area.base.x + target_area.dimensions.x - 1;
    if (drone.pos.x > target_x_end) {
        // We've passed the target area and there's no moving back.
        return true;
    }

    const auto target_y_bottom = target_area.base.y;
    if (drone.pos.y < target_y_bottom && drone.vel.y <= 0) {
        // We've fallen below the target area and can't get back up.
        return true;
    }

    return false;
}

auto drone_positions(velocity_t initial_velocity) noexcept
{
    // XXX For some reason ranges iota insists on the value type having a
    // difference_type?
    // return rv::iota(drone_t{start_pos, initial_velocity});

    drone_t drone{start_pos, initial_velocity};
    auto generator{[drone]() mutable { return drone++; }};
    return rv::generate(generator);
}

auto drone_positions_until_fail(velocity_t initial_velocity,
                                target_area_t target_area)
{
    auto hasnt_failed = [target_area](const drone_t& drone) {
        return !has_failed(drone, target_area);
    };
    return drone_positions(initial_velocity) | rv::take_while(hasnt_failed);
}

bool test_starting_velocity(velocity_t vel, target_area_t target_area)
{
    auto is_in_target_area{[&target_area](const drone_t& drone) {
        return target_area.contains(drone.pos);
    }};
    return r::any_of(drone_positions_until_fail(vel, target_area),
                     is_in_target_area);
}

int_t max_drone_elevation(velocity_t vel, target_area_t target_area)
{
    return r::max(drone_positions_until_fail(vel, target_area) |
                  rv::transform([](const drone_t& d) { return d.pos.y; }));
}

target_area_t parse_input(std::string_view line)
{
    line = trim(line);
    constexpr auto matcher{ctre::match<
        R"(target area: x=(-?\d+)\.\.(-?\d+), y=(-?\d+)\.\.(-?\d+))">};
    if (auto [whole, x1, x2, y1, y2] = matcher(line); whole) {
        const pos_t corner1{to_num<int_t>(x1.to_view()),
                            to_num<int_t>(y1.to_view())};
        const pos_t corner2{to_num<int_t>(x2.to_view()),
                            to_num<int_t>(y2.to_view())};
        // TODO: "point from whatever x1/y1 actually are" helper
        return {rect_from_corners(corner1, corner2)};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

}  // namespace

aoc::solution_result day17(std::string_view input)
{
    const target_area_t target_area{parse_input(input)};

    const int_t max_x{target_area.base.x + target_area.dimensions.x};
    const int_t min_y{target_area.base.y};
    const int_t max_y{std::abs(min_y) + 1};

    int_t greatest_max_elevation{0};
    int_t target_area_count{0};

    for (int_t x{0}; x <= max_x; x++) {
        for (int_t y{min_y}; y <= max_y; y++) {
            const velocity_t vel{x, y};
            if (test_starting_velocity(vel, target_area)) {
                greatest_max_elevation =
                    std::max(greatest_max_elevation,
                             max_drone_elevation(vel, target_area));
                target_area_count++;
            }
        }
    }

    return {greatest_max_elevation, target_area_count};
}

}  // namespace aoc::year2021
