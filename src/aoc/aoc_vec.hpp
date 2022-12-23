//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_VEC_HPP
#define AOC_VEC_HPP

#include "aoc.hpp"
#include "aoc_range.hpp"

#include <fmt/format.h>

#include <compare>
#include <functional>

namespace aoc {

template <typename Scalar>
struct vec2 {
    Scalar x{};
    Scalar y{};

    friend vec2 operator+(vec2 const& lhs, vec2 const& rhs)
    {
        return {static_cast<Scalar>(lhs.x + rhs.x),
                static_cast<Scalar>(lhs.y + rhs.y)};
    }
    friend vec2 operator-(vec2 const& lhs, vec2 const& rhs)
    {
        return {static_cast<Scalar>(lhs.x - rhs.x),
                static_cast<Scalar>(lhs.y - rhs.y)};
    }
    friend vec2& operator+=(vec2& lhs, const vec2& rhs)
    {
        return lhs = lhs + rhs;
    }
    friend vec2& operator-=(vec2& lhs, const vec2& rhs)
    {
        return lhs = lhs - rhs;
    }

    friend vec2 operator*(const vec2& lhs, const Scalar& rhs)
    {
        return {lhs.x * rhs, lhs.y * rhs};
    }
    friend vec2 operator*(const Scalar& lhs, const vec2& rhs)
    {
        return rhs * lhs;
    }
    friend vec2& operator*=(vec2& lhs, const Scalar& rhs)
    {
        return lhs = lhs * rhs;
    }
    friend vec2& operator*=(Scalar& lhs, const vec2& rhs)
    {
        return rhs = lhs * rhs;
    }

    friend auto operator<=>(const vec2&, const vec2&) = default;
};

template <typename Scalar>
vec2<Scalar> vec_from_strings(std::string_view x, std::string_view y) noexcept
{
    return {to_num<Scalar>(x), to_num<Scalar>(y)};
}

template <typename Scalar>
struct rect {
    vec2<Scalar> base{};
    vec2<Scalar> dimensions{};

    friend auto operator<=>(const rect& lhs,
                            const rect& rhs) noexcept = default;

    friend rect operator+(const rect& lhs, const vec2<Scalar>& rhs)
    {
        return {lhs.base + rhs, lhs.dimensions};
    }

    friend rect& operator+=(rect& lhs, const vec2<Scalar>& rhs)
    {
        return lhs = lhs + rhs;
    }

    auto all_points() const noexcept
    {
        static_assert(std::is_integral_v<Scalar>);
        return rv::cartesian_product(
                   rv::iota(base.y) | rv::take(dimensions.y),
                   rv::iota(base.x) | rv::take(dimensions.x)) |
               rv::transform([](auto p) {
                   return vec2<Scalar>{std::get<1>(p), std::get<0>(p)};
               });
    }

    bool contains(vec2<Scalar> const& point) const noexcept
    {
        return point.x >= base.x && point.x < base.x + dimensions.x &&
               point.y >= base.y && point.y < base.y + dimensions.y;
    }
};

template <typename Scalar>
rect<Scalar> rect_from_corners(const vec2<Scalar>& corner1,
                               const vec2<Scalar>& corner2) noexcept
{
    const vec2<Scalar> min{std::min(corner1.x, corner2.x),
                           std::min(corner1.y, corner2.y)};
    const vec2<Scalar> max{std::max(corner1.x, corner2.x),
                           std::max(corner1.y, corner2.y)};
    return {min, max - min + vec2<Scalar>{1, 1}};
}

template <typename Scalar>
rect<Scalar> rect_from_corner_strings(
    std::pair<std::string_view, std::string_view> corner1,
    std::pair<std::string_view, std::string_view> corner2) noexcept
{
    auto sv_to_vec{[](const auto pair) {
        return vec_from_strings<Scalar>(pair.first, pair.second);
    }};
    return rect_from_corners<Scalar>(sv_to_vec(corner1), sv_to_vec(corner2));
}

template <typename Scalar>
struct vec3 {
    Scalar x{};
    Scalar y{};
    Scalar z{};

    friend vec3 operator+(vec3 const& lhs, vec3 const& rhs)
    {
        return {static_cast<Scalar>(lhs.x + rhs.x),
                static_cast<Scalar>(lhs.y + rhs.y),
                static_cast<Scalar>(lhs.z + rhs.z)};
    }
    friend vec3 operator-(vec3 const& lhs, vec3 const& rhs)
    {
        return {static_cast<Scalar>(lhs.x - rhs.x),
                static_cast<Scalar>(lhs.y - rhs.y),
                static_cast<Scalar>(lhs.z - rhs.z)};
    }
    friend vec3& operator+=(vec3& lhs, const vec3& rhs)
    {
        return lhs = lhs + rhs;
    }
    friend vec3& operator-=(vec3& lhs, const vec3& rhs)
    {
        return lhs = lhs - rhs;
    }

    friend auto operator<=>(const vec3&, const vec3&) = default;
};

template <typename Scalar>
vec3<Scalar> vec_from_strings(std::string_view x,
                              std::string_view y,
                              std::string_view z) noexcept
{
    return {to_num<Scalar>(x), to_num<Scalar>(y), to_num<Scalar>(z)};
}

}  // namespace aoc

template <typename Scalar>
struct std::hash<aoc::vec2<Scalar>> {
    std::size_t operator()(const aoc::vec2<Scalar>& v) const noexcept
    {
        std::size_t h1 = std::hash<Scalar>{}(v.x);
        std::size_t h2 = std::hash<Scalar>{}(v.y);
        return h1 ^ (h2 << 1);
    }
};

template <typename Scalar>
struct std::hash<aoc::vec3<Scalar>> {
    std::size_t operator()(const aoc::vec3<Scalar>& v) const noexcept
    {
        // XXX This is not a great hash function
        std::size_t h1 = std::hash<Scalar>{}(v.x);
        std::size_t h2 = std::hash<Scalar>{}(v.y);
        std::size_t h3 = std::hash<Scalar>{}(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

template <typename Scalar>
struct fmt::formatter<aoc::vec2<Scalar>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        // No formatting options for this type.
        const auto it{ctx.begin()};
        if (it != ctx.end() && *it != '}') {
            throw format_error("invalid format");
        }
        return it;
    }

    template <typename FormatContext>
    auto format(const aoc::vec2<Scalar>& p, FormatContext& ctx)
        -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{},{}", p.x, p.y);
    }
};

#endif  // AOC_VEC_HPP
