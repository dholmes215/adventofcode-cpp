//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc_range.hpp>

#include <catch2/catch.hpp>

#include <string_view>

using namespace aoc;

TEST_CASE("convert lines to ints", "[to_int]")
{
    const std::string_view str{"199\n203\n200\n"};
    const auto int_vec{int_lines(str) | r::to<std::vector>()};
    std::vector<int> expected{199,203,200};
    CHECK(int_vec == expected);
}
