//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc_grid.hpp>
#include <aoc_range.hpp>

#include <catch2/catch.hpp>

#include <string>

using namespace aoc;

namespace {
auto str(auto&& rng)
{
    return rng | r::to<std::string>();
}
}  // namespace

TEST_CASE("grid_rows", "[grid]")
{
    std::string array{"abcdefghi"};
    auto rows{grid_rows(array, 3)};
    CHECK(rows.size() == 3);
    CHECK(sv(rows[0]) == "abc");
    CHECK(sv(rows[1]) == "def");
    CHECK(sv(rows[2]) == "ghi");
    r::reverse(rows[1]);
    CHECK(sv(rows[1]) == "fed");
    // r::reverse(rows); // would be cool
    CHECK(sv((rows | rv::reverse)[0]) == "ghi");
    CHECK(sv((rows | rv::reverse)[1]) == "fed");
    CHECK(sv((rows | rv::reverse)[2]) == "abc");
}

TEST_CASE("grid_rows uneven", "[grid]")
{
    std::string array{"abcdefg"};
    auto rows{grid_rows(array, 3)};
    CHECK(rows.size() == 3);
    CHECK(sv(rows[0]) == "abc");
    CHECK(sv(rows[1]) == "def");
    CHECK(sv(rows[2]) == "g");
    CHECK(r::distance(rows[0]) == 3);
    CHECK(r::distance(rows[1]) == 3);
    CHECK(r::distance(rows[2]) == 1);
    r::reverse(rows[1]);
    CHECK(sv(rows[1]) == "fed");
    r::reverse(rows[2]);
    CHECK(sv(rows[2]) == "g");
    // r::reverse(rows); // would be cool
    CHECK(sv((rows | rv::reverse)[0]) == "g");
    CHECK(sv((rows | rv::reverse)[1]) == "fed");
    CHECK(sv((rows | rv::reverse)[2]) == "abc");
}

TEST_CASE("grid_rows_large", "[grid]")
{
    constexpr int width{1000};
    constexpr int height{width};
    std::vector<char> chars{};
    chars.resize(width * height);
    r::copy(rv::iota('a') | rv::take(26) | rv::cycle | rv::take(width * height),
            chars.data());

    auto rows{grid_rows(chars, width)};
    CHECK(rows.size() == height);

    CHECK(sv(rows[50] | rv::take(20)) == "cdefghijklmnopqrstuv");
    // CHECK(sv(rows[1]) == "def");
    // CHECK(sv(rows[2]) == "ghi");
    // r::reverse(rows[1]);
    // CHECK(sv(rows[1]) == "fed");
    // // r::reverse(rows); // would be cool
    // CHECK(sv((rows | rv::reverse)[0]) == "ghi");
    // CHECK(sv((rows | rv::reverse)[1]) == "fed");
    // CHECK(sv((rows | rv::reverse)[2]) == "abc");
}

TEST_CASE("grid_row", "[grid]")
{
    std::string array{"abcdefghi"};
    auto row{grid_row(array, 3, 1)};
    CHECK(sv(row) == "def");
    r::reverse(row);
    CHECK(sv(row) == "fed");
}

TEST_CASE("grid_cols", "[grid]")
{
    std::string array{"abcdefghi"};
    auto cols{grid_cols(array, 3)};
    CHECK(cols.size() == 3);
    CHECK(str(cols[0]) == "adg");
    CHECK(str(cols[1]) == "beh");
    CHECK(str(cols[2]) == "cfi");
    r::reverse(cols[1]);
    CHECK(str(cols[1]) == "heb");
    // r::reverse(cols); // would be cool
    CHECK(str((cols | rv::reverse)[0]) == "cfi");
    CHECK(str((cols | rv::reverse)[1]) == "heb");
    CHECK(str((cols | rv::reverse)[2]) == "adg");
}

TEST_CASE("grid_cols uneven", "[grid]")
{
    std::string array{"abcdefg"};
    auto cols{grid_cols(array, 3)};
    CHECK(cols.size() == 3);
    CHECK(str(cols[0]) == "adg");
    CHECK(str(cols[1]) == "be");
    CHECK(str(cols[2]) == "cf");
    CHECK(r::distance(cols[0]) == 3);
    CHECK(r::distance(cols[1]) == 2);
    CHECK(r::distance(cols[2]) == 2);
    r::reverse(cols[0]);
    CHECK(str(cols[0]) == "gda");
    r::reverse(cols[2]);
    CHECK(str(cols[2]) == "fc");
    // r::reverse(cols); // would be cool
    CHECK(str((cols | rv::reverse)[0]) == "fc");
    CHECK(str((cols | rv::reverse)[1]) == "be");
    CHECK(str((cols | rv::reverse)[2]) == "gda");
}

TEST_CASE("grid_col", "[grid]")
{
    std::string array{"abcdefghi"};
    auto col{grid_col(array, 3, 1)};
    CHECK(str(col) == "beh");
    r::reverse(col);
    CHECK(str(col) == "heb");
}

TEST_CASE("grid_adapter", "[grid]")
{
    std::array<bool, 8 * 8> array{0};
    grid_adapter<decltype(array), 8> grid{array};
    CHECK(r::count(array, false) == 64);
    grid[{1, 2}] = true;
    CHECK(r::count(array, false) == 63);
    CHECK(array[17] == true);
}

template <class>
struct DNE;

TEST_CASE("subgrid", "[grid]")
{
    std::string s{"abcdefghijklmnopqrstuvwxy"};
    // abcde
    // fghij
    // klmno
    // pqrst
    // uvwxy
    grid_adapter<decltype(s), 5> grid{s};
    auto sub{grid.subgrid({{1, 1}, {2, 3}})};
    // gh
    // lm
    // qr
    CHECK(sub.row(2).size() == 2);
    CHECK(sv(sub.row(0)) == "gh");
    CHECK(sv(sub.row(1)) == "lm");
    CHECK(sv(sub.row(2)) == "qr");
    CHECK(sub.col(1).size() == 3);
    CHECK(str(sub.col(0)) == "glq");
    CHECK(str(sub.col(1)) == "hmr");
    r::reverse(sub.row(1));
    CHECK(sv(sub.row(1)) == "ml");
    r::reverse(sub.col(1));
    CHECK(str(sub.col(1)) == "rlh");
    CHECK(str(sub.data()) == "grmlqh");
    CHECK(s == "abcdefgrijkmlnopqhstuvwxy");

    auto sub2{grid.subgrid({{2, 2}, {2, 2}})};
    auto data{sub2.data()};
    r::fill(data, '1');
    CHECK(s == "abcdefgrijkm11opq11tuvwxy");

    std::array<bool, 4 * 4> a{0};
    grid_adapter<decltype(a), 4> grid3{a};
    auto sub3{grid3.subgrid({{1, 1}, {2, 2}})};
    sub3[{1, 2}] = true;
    sub3[{2, 1}] = true;
    CHECK(grid3[{2, 3}] == true);
    CHECK(grid3[{3, 2}] == true);
}
