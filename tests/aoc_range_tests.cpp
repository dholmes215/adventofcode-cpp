//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc_range.hpp>

#include <catch2/catch_all.hpp>

#include <string_view>

using namespace aoc;

TEST_CASE("convert lines to ints", "[to_int]")
{
    const std::string_view str{"199\n203\n200\n"};
    const auto int_vec{int_lines(str) | r::to<std::vector>()};
    std::vector<int> expected{199,203,200};
    CHECK(int_vec == expected);
}

TEST_CASE("convert bool ranges to ints", "[bool_range_to_int]")
{
    std::array a26986{false, true, true, false, true, false, false, true,
                      false, true, true, false, true, false, true,  false};
    CHECK(bool_range_to_int(a26986) == 26986);
}
