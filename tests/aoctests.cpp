//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>

#include <catch2/catch_all.hpp>

using namespace aoc;

TEST_CASE("trim assorted strings", "[trim]")
{
    CHECK(trim("") == "");
    CHECK(trim("   ") == "");
    CHECK(trim(" \n ") == "");
    CHECK(trim("begin ") == "begin");
    CHECK(trim(" end") == "end");
    CHECK(trim(" middle ") == "middle");
    CHECK(trim("\nbetween lines\n") == "between lines");
}
