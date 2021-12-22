//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <ctre.hpp>
#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

namespace {

}  // namespace

template <typename Scalar>
struct vec3 {
    std::array<Scalar, 3> components{0};

    constexpr auto& operator[](std::size_t i) { return components[i]; }
    constexpr auto operator[](std::size_t i) const { return components[i]; }

    constexpr friend vec3 operator+(const vec3& lhs, const vec3& rhs) noexcept
    {
        return {lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]};
    }

    constexpr vec3& operator+=(const vec3& rhs) noexcept
    {
        components[0] += rhs[0];
        components[1] += rhs[1];
        components[2] += rhs[2];
        return *this;
    }

    constexpr friend vec3 operator-(const vec3& lhs, const vec3& rhs) noexcept
    {
        return {lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]};
    }

    constexpr vec3& operator-=(const vec3& rhs) noexcept
    {
        components[0] -= rhs[0];
        components[1] -= rhs[1];
        components[2] -= rhs[2];
        return *this;
    }

    constexpr friend auto operator<=>(const vec3& lhs,
                                      const vec3& rhs) noexcept = default;
};

using scalar_t = int;
using vec_t = vec3<scalar_t>;
constexpr std::size_t grid_size_a{101};
constexpr vec_t origin{50, 50, 50};
using grid3_a_t =
    std::array<std::array<std::array<bool, grid_size_a>, grid_size_a>,
               grid_size_a>;

constexpr std::size_t grid_size_b{840};
using grid3_b_t =
    std::array<std::array<std::array<bool, grid_size_b>, grid_size_b>,
               grid_size_b>;

struct instruction {
    bool on;
    vec_t min_point;
    vec_t max_point;

    bool contains(const vec_t& point) const noexcept
    {
        return (point[0] >= min_point[0] && point[0] <= max_point[0]) &&
               (point[1] >= min_point[1] && point[1] <= max_point[1]) &&
               (point[2] >= min_point[2] && point[2] <= max_point[2]);
    }
};

instruction parse_instruction(std::string_view line)
{
    constexpr auto matcher{ctre::match<
        R"((off|on) x=(-?\d+)\.\.(-?\d+),y=(-?\d+)\.\.(-?\d+),z=(-?\d+)\.\.(-?\d+))">};
    if (auto [whole, on, x1, x2, y1, y2, z1, z2] = matcher(line); whole) {
        return {
            on == "on",
            {to_num<scalar_t>(x1), to_num<scalar_t>(y1), to_num<scalar_t>(z1)},
            {to_num<scalar_t>(x2), to_num<scalar_t>(y2), to_num<scalar_t>(z2)}};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

aoc::solution_result day22(std::string_view input)
{
    const auto instructions =
        sv_lines(input) | rv::transform(parse_instruction) | r::to<std::vector>;

    std::unique_ptr<grid3_a_t> grid_a_ptr = std::make_unique<grid3_a_t>();
    grid3_a_t& grid_a = *grid_a_ptr;

    for (const auto& inst : instructions) {
        if (inst.min_point[0] < -50 || inst.max_point[0] > 50 ||
            inst.min_point[1] < -50 || inst.max_point[1] > 50 ||
            inst.min_point[2] < -50 || inst.max_point[2] > 50) {
            continue;
        }

        for (auto x = inst.min_point[0]; x <= inst.max_point[0]; ++x) {
            for (auto y = inst.min_point[1]; y <= inst.max_point[1]; ++y) {
                for (auto z = inst.min_point[2]; z <= inst.max_point[2]; ++z) {
                    const vec_t point{x, y, z};
                    if (inst.contains(point)) {
                        const vec_t adjusted_point{point + origin};
                        grid_a[static_cast<std::size_t>(adjusted_point[0])]
                              [static_cast<std::size_t>(adjusted_point[1])]
                              [static_cast<std::size_t>(adjusted_point[2])] =
                                  inst.on;
                    }
                }
            }
        }
    }

    auto part1_on{r::count(grid_a | rv::join | rv::join, true)};

    // Identify the beginnings (inclusive) and ends (exclusive) of each region
    // on each axis
    std::vector<int> x_boundaries;
    for (const auto& instruction : instructions) {
        x_boundaries.push_back(instruction.min_point[0]);
        x_boundaries.push_back(instruction.max_point[0] + 1);
    }
    std::sort(x_boundaries.begin(), x_boundaries.end());
    auto x_last{std::unique(x_boundaries.begin(), x_boundaries.end())};
    x_boundaries.erase(x_last, x_boundaries.end());

    std::vector<int> y_boundaries;
    for (const auto& instruction : instructions) {
        y_boundaries.push_back(instruction.min_point[1]);
        y_boundaries.push_back(instruction.max_point[1] + 1);
    }
    std::sort(y_boundaries.begin(), y_boundaries.end());
    auto y_last{std::unique(y_boundaries.begin(), y_boundaries.end())};
    y_boundaries.erase(y_last, y_boundaries.end());

    std::vector<int> z_boundaries;
    for (const auto& instruction : instructions) {
        z_boundaries.push_back(instruction.min_point[2]);
        z_boundaries.push_back(instruction.max_point[2] + 1);
    }
    std::sort(z_boundaries.begin(), z_boundaries.end());
    auto z_last{std::unique(z_boundaries.begin(), z_boundaries.end())};
    z_boundaries.erase(z_last, z_boundaries.end());

    std::unique_ptr<grid3_b_t> grid_b_ptr = std::make_unique<grid3_b_t>();
    grid3_b_t& grid_b = *grid_b_ptr;

    for (const auto& inst : instructions) {
        const auto x_min_it{std::lower_bound(
            x_boundaries.begin(), x_boundaries.end(), inst.min_point[0])};
        const auto x_max_it{std::upper_bound(
            x_boundaries.begin(), x_boundaries.end(), inst.max_point[0])};
        const auto x_min_idx{x_min_it - x_boundaries.begin()};
        const auto x_max_idx{x_max_it - x_boundaries.begin()};

        for (auto x{x_min_idx}; x < x_max_idx; x++) {
            const auto y_min_it{std::lower_bound(
                y_boundaries.begin(), y_boundaries.end(), inst.min_point[1])};
            const auto y_max_it{std::upper_bound(
                y_boundaries.begin(), y_boundaries.end(), inst.max_point[1])};
            const auto y_min_idx{y_min_it - y_boundaries.begin()};
            const auto y_max_idx{y_max_it - y_boundaries.begin()};

            for (auto y{y_min_idx}; y < y_max_idx; y++) {
                const auto z_min_it{std::lower_bound(z_boundaries.begin(),
                                                     z_boundaries.end(),
                                                     inst.min_point[2])};
                const auto z_max_it{std::upper_bound(z_boundaries.begin(),
                                                     z_boundaries.end(),
                                                     inst.max_point[2])};
                const auto z_min_idx{z_min_it - z_boundaries.begin()};
                const auto z_max_idx{z_max_it - z_boundaries.begin()};

                for (auto z{z_min_idx}; z < z_max_idx; z++) {
                    const vec_t point{static_cast<int>(x), static_cast<int>(y),
                                      static_cast<int>(z)};

                    grid_b[static_cast<std::size_t>(point[0])]
                          [static_cast<std::size_t>(point[1])]
                          [static_cast<std::size_t>(point[2])] = inst.on;
                }
            }
        }
    }

    std::uint64_t part2_on{0};

    for (std::size_t x{0}; x < x_boundaries.size() - 1; x++) {
        for (std::size_t y{0}; y < y_boundaries.size() - 1; y++) {
            for (std::size_t z{0}; z < z_boundaries.size() - 1; z++) {
                if (grid_b[x][y][z]) {
                    auto x_dim{static_cast<std::uint64_t>(x_boundaries[x + 1] -
                                                          x_boundaries[x])};
                    auto y_dim{static_cast<std::uint64_t>(y_boundaries[y + 1] -
                                                          y_boundaries[y])};
                    auto z_dim{static_cast<std::uint64_t>(z_boundaries[z + 1] -
                                                          z_boundaries[z])};
                    part2_on += x_dim * y_dim * z_dim;
                }
            }
        }
    }

    return {part1_on, part2_on};
}

}  // namespace aoc::year2021
