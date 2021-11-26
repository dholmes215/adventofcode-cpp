//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_VEC_HPP
#define AOC_VEC_HPP

#include <compare>

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
    friend bool operator==(const vec2&, const vec2&) = default;
};

}  // namespace aoc

#endif  // AOC_VEC_HPP
