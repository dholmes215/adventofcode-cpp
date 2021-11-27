//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>

#include <catch2/catch.hpp>

using namespace aoc;

TEST_CASE("strip assorted strings", "[strip]")
{
    CHECK(strip("") == "");
    CHECK(strip("   ") == "");
    CHECK(strip(" \n ") == "");
    CHECK(strip("begin ") == "begin");
    CHECK(strip(" end") == "end");
    CHECK(strip(" middle ") == "middle");
    CHECK(strip("\nbetween lines\n") == "between lines");
}
