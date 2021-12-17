//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_VEC_HPP
#define AOC_VEC_HPP

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

    friend auto operator<=>(const vec2&, const vec2&) = default;
};

template <typename Scalar>
struct rect {
    vec2<Scalar> top_left{};
    vec2<Scalar> dimensions{};

    friend auto operator<=>(const rect& lhs,
                            const rect& rhs) noexcept = default;

    friend rect operator+(const rect& lhs, const vec2<Scalar>& rhs)
    {
        return {lhs.top_left + rhs, lhs.dimensions};
    }

    friend rect& operator+=(rect& lhs, const vec2<Scalar>& rhs)
    {
        return lhs = lhs + rhs;
    }

    auto all_points() const noexcept
    {
        static_assert(std::is_integral_v<Scalar>);
        return rv::cartesian_product(
                   rv::iota(top_left.y) | rv::take(dimensions.y),
                   rv::iota(top_left.x) | rv::take(dimensions.x)) |
               rv::transform([](auto p) {
                   return vec2<Scalar>{std::get<1>(p), std::get<0>(p)};
               });
    }

    bool contains(vec2<Scalar> const& point) const noexcept
    {
        return point.x >= top_left.x && point.x < top_left.x + dimensions.x &&
               point.y >= top_left.y && point.y < top_left.y + dimensions.y;
    }
};

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
        return format_to(ctx.out(), "{},{}", p.x, p.y);
    }
};

#endif  // AOC_VEC_HPP
