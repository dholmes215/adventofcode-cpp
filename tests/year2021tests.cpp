//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <bits.hpp>
#include <snailfish.hpp>

#include <aoc_range.hpp>

#include <fmt/format.h>
#include <catch2/catch.hpp>

using namespace aoc;
using namespace aoc::year2021;

namespace {

std::vector<bit_t> vec(std::string_view s)
{
    return rv::transform(s, [](char c) { return bit_t{c == '1'}; }) |
           r::to<std::vector>;
}

}  // namespace

TEST_CASE("2021 BITS hex_to_bit_range", "[2021-16]")
{
    CHECK((hex_to_bit_range('0') | r::to<std::vector>) == vec("0000"));
    CHECK((hex_to_bit_range('1') | r::to<std::vector>) == vec("0001"));
    CHECK((hex_to_bit_range('2') | r::to<std::vector>) == vec("0010"));
    CHECK((hex_to_bit_range('3') | r::to<std::vector>) == vec("0011"));
    CHECK((hex_to_bit_range('4') | r::to<std::vector>) == vec("0100"));
    CHECK((hex_to_bit_range('5') | r::to<std::vector>) == vec("0101"));
    CHECK((hex_to_bit_range('6') | r::to<std::vector>) == vec("0110"));
    CHECK((hex_to_bit_range('7') | r::to<std::vector>) == vec("0111"));
    CHECK((hex_to_bit_range('8') | r::to<std::vector>) == vec("1000"));
    CHECK((hex_to_bit_range('9') | r::to<std::vector>) == vec("1001"));
    CHECK((hex_to_bit_range('A') | r::to<std::vector>) == vec("1010"));
    CHECK((hex_to_bit_range('B') | r::to<std::vector>) == vec("1011"));
    CHECK((hex_to_bit_range('C') | r::to<std::vector>) == vec("1100"));
    CHECK((hex_to_bit_range('D') | r::to<std::vector>) == vec("1101"));
    CHECK((hex_to_bit_range('E') | r::to<std::vector>) == vec("1110"));
    CHECK((hex_to_bit_range('F') | r::to<std::vector>) == vec("1111"));
}

TEST_CASE("2021 BITS hex_to_bit_vector", "[2021-16]")
{
    CHECK((hex_to_bit_vector("D2FE28") | r::to<std::vector>) ==
          vec("110100101111111000101000"));
    CHECK((hex_to_bit_vector("38006F45291200") | r::to<std::vector>) ==
          vec("00111000000000000110111101000101001010010001001000000000"));
    CHECK((hex_to_bit_vector("EE00D40C823060") | r::to<std::vector>) ==
          vec("11101110000000001101010000001100100000100011000001100000"));
}

TEST_CASE("2021 BITS bit_range_to_int", "[2021-16]")
{
    CHECK(bit_range_to_int(vec("0")) == 0);
    CHECK(bit_range_to_int(vec("1")) == 1);
    CHECK(bit_range_to_int(vec("00000000000000000000000000000000000000000000000"
                               "00000000000000000")) == 0);
    CHECK(bit_range_to_int(vec("00000000000000000000000000000000000000000000000"
                               "00000000000000001")) == 1);
    CHECK(
        bit_range_to_int(vec("10000000000000000000000000000000000000000000000"
                             "00000000000000000")) ==
        0b1000000000000000000000000000000000000000000000000000000000000000ULL);
    CHECK(
        bit_range_to_int(vec("1111111111111111111111111111111111111111111111111"
                             "111111111111111")) ==
        0b1111111111111111111111111111111111111111111111111111111111111111ULL);
}

TEST_CASE("2021 BITS read_literal", "[2021-16]")
{
    bit_iter_t iter;

    const auto zero{vec("00000")};
    iter = zero.begin();
    CHECK(read_literal(iter) == 0);
    CHECK(r::distance(zero.begin(), iter) == 5);

    const auto _2021{vec("101111111000101000")};
    iter = _2021.begin();
    CHECK(read_literal(iter) == 2021);
    CHECK(r::distance(_2021.begin(), iter) == 15);

    const auto big{
        vec("111111111111111111111111111111111111111111111111111111111111111111"
            "11111111101111")};
    iter = big.begin();
    CHECK(
        read_literal(iter) ==
        0b1111111111111111111111111111111111111111111111111111111111111111ULL);
    CHECK(r::distance(big.begin(), iter) == 80);
}

TEST_CASE("2021 snailfish operator+", "[2021-18]")
{
    snail_num_t onetwo{1, 2};
    snail_num_t threefour{3, 4};
    CHECK((onetwo + threefour) == parse_snail("[[1,2],[3,4]]"));
    snail_num_t oneten{1, 10};  // Not really valid
    CHECK((oneten + threefour) == parse_snail("[[1,[5,5]],[3,4]]"));

    snail_num_t left{parse_snail("[[[[4,3],4],4],[7,[[8,4],9]]]")};
    snail_num_t right{parse_snail("[1,1]")};
    CHECK(fmt::format("{}", (left + right)) ==
          fmt::format("{}", parse_snail("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]")));
}

TEST_CASE("2021 snailfish operator==", "[2021-18]")
{
    snail_num_t left{1, 2};
    snail_num_t right{3, 4};
    CHECK(parse_snail("[[1,2],[3,4]]") == parse_snail("[[1,2],[3,4]]"));
    CHECK(!(parse_snail("[[1,2],[3,4]]") == parse_snail("[[1,2],[3,2]]")));
}

TEST_CASE("2021 snailfish operator!=", "[2021-18]")
{
    snail_num_t left{1, 2};
    snail_num_t right{3, 4};
    CHECK(!(parse_snail("[[1,2],[3,4]]") != parse_snail("[[1,2],[3,4]]")));
    CHECK(parse_snail("[[1,2],[3,4]]") != parse_snail("[[1,2],[3,2]]"));
}

TEST_CASE("2021 snailfish magnitude", "[2021-18]")
{
    CHECK(parse_snail("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]")
              .magnitude() == 3488);
}