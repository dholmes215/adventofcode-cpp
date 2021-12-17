//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <catch2/catch.hpp>

#include <fmt/format.h>

#include <vector>

using namespace aoc;

TEST_CASE("rect::all_points()", "[vec]")
{
    rect<int> r1{{0, 0}, {3, 3}};
    std::vector<vec2<int>> v1{{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1},
                              {2, 1}, {0, 2}, {1, 2}, {2, 2}};
    std::vector<vec2<int>> a1{r1.all_points() | r::to<std::vector>};
    CHECK(a1 == v1);

    rect<int> r2{{-1, -1}, {3, 4}};
    std::vector<vec2<int>> v2{{-1, -1}, {0, -1}, {1, -1}, {-1, 0},
                              {0, 0},   {1, 0},  {-1, 1}, {0, 1},
                              {1, 1},   {-1, 2}, {0, 2},  {1, 2}};
    std::vector<vec2<int>> a2{r2.all_points() | r::to<std::vector>};
    CHECK(a2 == v2);
}

TEST_CASE("vec formatter", "[vec]")
{
    vec2<int> v1{1, 2};
    CHECK(fmt::format("{}", v1) == "1,2");
}