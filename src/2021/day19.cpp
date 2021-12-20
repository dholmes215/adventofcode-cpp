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
#include <optional>
#include <set>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

namespace {

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

template <typename Scalar>
struct matrix3 {
    using row_t = std::array<Scalar, 3>;
    std::array<row_t, 3> rows{{{0}, {0}, {0}}};

    constexpr friend matrix3 operator*(const matrix3& lhs,
                                       const matrix3& rhs) noexcept
    {
        matrix3 result;
        for (std::size_t i = 0; i < 3; ++i) {
            for (std::size_t j = 0; j < 3; ++j) {
                for (std::size_t k = 0; k < 3; ++k) {
                    result.rows[i][j] += lhs.rows[i][k] * rhs.rows[k][j];
                }
            }
        }
        return result;
    }

    constexpr friend vec3<Scalar> operator*(const matrix3& lhs,
                                            const vec3<Scalar>& rhs) noexcept
    {
        vec3<Scalar> result;
        for (std::size_t i = 0; i < 3; ++i) {
            for (std::size_t j = 0; j < 3; ++j) {
                result[i] += lhs.rows[i][j] * rhs[j];
            }
        }
        return result;
    }

    // Treat this as a function that applies rotation to a vector for
    // convenience.
    constexpr vec3<Scalar> operator()(const vec3<Scalar>& rhs) const noexcept
    {
        return *this * rhs;
    }

    constexpr friend auto operator<=>(const matrix3& lhs,
                                      const matrix3& rhs) noexcept = default;
};

using scalar_t = int;
using position_t = vec3<scalar_t>;
using matrix_t = matrix3<scalar_t>;
using degrees_t = int;

constexpr position_t origin{{0, 0, 0}};

constexpr matrix_t identity{{{
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
}}};

static_assert(identity * identity == identity);

constexpr scalar_t cos(degrees_t angle)
{
    switch (angle % 360) {
        case 0:
            return 1;
        case 90:
            return 0;
        case 180:
            return -1;
        case 270:
            return 0;
    }
    throw std::domain_error("invalid right angle");
}

constexpr scalar_t sin(degrees_t angle)
{
    switch (angle % 360) {
        case 0:
            return 0;
        case 90:
            return 1;
        case 180:
            return 0;
        case 270:
            return -1;
    }
    throw std::domain_error("invalid right angle");
}

constexpr matrix_t roll(const degrees_t angle)
{
    return {{{{1, 0, 0},
              {0, cos(angle), -sin(angle)},
              {0, sin(angle), cos(angle)}}}};
}

constexpr matrix_t pitch(const degrees_t angle)
{
    return {{{{cos(angle), 0, sin(angle)},
              {0, 1, 0},
              {-sin(angle), 0, cos(angle)}}}};
}

constexpr matrix_t yaw(const degrees_t angle)
{
    return {{{{cos(angle), -sin(angle), 0},
              {sin(angle), cos(angle), 0},
              {0, 0, 1}}}};
}

static_assert((yaw(90) * position_t{1, 0, 0}) == position_t{0, 1, 0});

// clang-format off
constexpr std::array<matrix_t, 24> orientations{{
    identity,
    roll(90),
    roll(180),
    roll(270),
    yaw(180),
    yaw(180) * roll(90),
    yaw(180) * roll(180),
    yaw(180) * roll(270),
    pitch(90),
    pitch(90) * roll(90),
    pitch(90) * roll(180),
    pitch(90) * roll(270),
    pitch(270),
    pitch(270) * roll(90),
    pitch(270) * roll(180),
    pitch(270) * roll(270),
    yaw(90),
    yaw(90) * roll(90),
    yaw(90) * roll(180),
    yaw(90) * roll(270),
    yaw(270),
    yaw(270) * roll(90),
    yaw(270) * roll(180),
    yaw(270) * roll(270),
}};
// clang-format on

// Parse input
position_t parse_position(std::string_view line)
{
    position_t out;
    r::copy(sv_split_range(line, ',') | rv::transform(to_num<scalar_t>),
            out.components.begin());
    return out;
}

int parse_scanner_id(std::string_view line)
{
    constexpr auto matcher{ctre::match<R"(--- scanner (\d+) ---)">};
    if (auto [whole, id] = matcher(line); whole) {
        return to_num<int>(id);
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

struct scanner_data {
    int scanner_id;
    std::set<position_t> beacons;
};

scanner_data parse_scanner_data(auto&& scanner_lines)
{
    return {parse_scanner_id(scanner_lines.front()),
            scanner_lines | rv::drop(1) | rv::transform(parse_position) |
                r::to<std::set>};
}

// camera position/orientation/state

struct scanner_delta {
    matrix_t orientation_delta;
    position_t position_delta;
};

scanner_data adjust_scanner_data(const scanner_data& data,
                                 const scanner_delta& delta)
{
    return {data.scanner_id, data.beacons |
                                 rv::transform(delta.orientation_delta) |
                                 rv::transform([&](const position_t& p) {
                                     return p + delta.position_delta;
                                 }) |
                                 r::to<std::set>};
}

std::optional<scanner_delta> compare_scanners(const scanner_data& a,
                                              const scanner_data& b)
{
    for (const matrix_t& orientation : orientations) {
        std::vector<position_t> b_rotated{
            b.beacons | rv::transform(orientation) | r::to<std::vector>};

        // Assume this is the correct orientation.
        // Try aligning with each point in a.

        for (const position_t& a_position : a.beacons) {
            for (const position_t& b_position : b_rotated) {
                position_t delta{a_position - b_position};

                std::vector<position_t> b_shifted{
                    b_rotated | rv::transform([delta](const position_t& p) {
                        return p + delta;
                    }) |
                    r::to<std::vector>};

                std::size_t count{0};
                for (const position_t& b_pos : b_shifted) {
                    count += a.beacons.count(b_pos);
                }

                if (count >= 12) {
                    return scanner_delta{orientation, delta};
                }
            }
        }
    }
    return {};
}

}  // namespace

aoc::solution_result day19(std::string_view input)
{
    const std::vector<scanner_data> scanner_groups{
        sv_lines(input) | rv::split("") |
        rv::transform([](auto&& rng) { return parse_scanner_data(rng); }) |
        r::to<std::vector>};

    std::set<int> unadjusted_scanners{
        scanner_groups |
        rv::transform([](auto& data) { return data.scanner_id; }) |
        r::to<std::set>};
    unadjusted_scanners.erase(0);

    std::map<int, position_t> scanner_positions;
    scanner_positions[0] = origin;

    std::map<int, scanner_data> scanner_data_adjusted_to_zero;
    scanner_data_adjusted_to_zero[0] = scanner_groups[0];

    while (!unadjusted_scanners.empty()) {
        for (const auto& [id, group_a] : scanner_data_adjusted_to_zero) {
            for (const auto& group_b : scanner_groups) {
                if (unadjusted_scanners.contains(group_b.scanner_id)) {
                    std::optional<scanner_delta> delta{
                        compare_scanners(group_a, group_b)};
                    if (delta) {
                        scanner_data_adjusted_to_zero[group_b.scanner_id] =
                            adjust_scanner_data(group_b, *delta);
                        unadjusted_scanners.erase(group_b.scanner_id);
                        scanner_positions[group_b.scanner_id] =
                            delta->position_delta;
                    }
                }
            }
        }
    }

    std::set<position_t> beacons_relative_to_zero;
    for (const auto& [id, group] : scanner_data_adjusted_to_zero) {
        beacons_relative_to_zero.insert(group.beacons.begin(),
                                        group.beacons.end());
    }

    auto manhattan_distance{[](const position_t& p, const position_t& q) {
        return std::abs(p[0] - q[0]) + std::abs(p[1] - q[1]) +
               std::abs(p[2] - q[2]);
    }};

    int largest_distance{
        r::max(rv::cartesian_product(scanner_positions | rv::values,
                                     scanner_positions | rv::values) |
               rv::transform([&](const auto& p) {
                   return manhattan_distance(std::get<0>(p), std::get<1>(p));
               }))};

    return {beacons_relative_to_zero.size(), largest_distance};
}

}  // namespace aoc::year2021
