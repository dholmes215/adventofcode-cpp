//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <day06.hpp>

extern "C" {
#include <md5.h>
}

#include <fmt/format.h>
#include <catch2/catch.hpp>

namespace aoc {
namespace {
std::string byte_to_hex(unsigned char byte)
{
    return fmt::format("{:02x}", byte);
}
std::string to_hex(const unsigned char* bytes, size_t size)
{
    auto hexes{subrange(bytes, bytes + size) | transform(byte_to_hex) |
               to<std::vector>()};
    auto joined = hexes | join;
    return joined | to<std::string>;
}
}  // namespace
}  // namespace aoc

TEST_CASE("md5 hashes are computed correctly", "[md5]")
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, "abcdef609043",
               static_cast<unsigned long>(strlen("abcdef609043")));
    unsigned char digest[16];
    MD5_Final(digest, &ctx);

    CHECK(aoc::to_hex(digest, 16) == "000001dbbfa3a5c83a2d506429c7b00e");
}

using namespace aoc::year2015::lights;
TEST_CASE("2015 day 06 regex", "[2015-06]")
{
    CHECK(string_to_instruction("turn on 0,0 through 999,999") ==
          instruction{light_action::on, {{0, 0}, {1000, 1000}}});
    CHECK(string_to_instruction("turn off 660,55 through 986,197") ==
          instruction{light_action::off, {{660, 55}, {327, 143}}});
    CHECK(string_to_instruction("turn on 226,196 through 599,390") ==
          instruction{light_action::on, {{226, 196}, {374, 195}}});
    CHECK(string_to_instruction("toggle 322,558 through 977,958") ==
          instruction{light_action::toggle, {{322, 558}, {656, 401}}});
    CHECK_THROWS_AS(string_to_instruction("twiddle 0,0 through 42,21"),
                    aoc::input_error);
}
