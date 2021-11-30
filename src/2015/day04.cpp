//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

extern "C" {
#include <md5.h>
}

#include <fmt/format.h>

#include <array>
#include <string>
#include <utility>

namespace aoc::year2015 {

namespace {

using Digest = std::array<uint8_t, 16>;
Digest md5(std::string_view input)
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    if (!std::in_range<unsigned long>(input.size())) {
        throw input_error("input too large");
    }
    MD5_Update(&ctx, input.data(), static_cast<unsigned long>(input.size()));
    Digest digest;
    MD5_Final(digest.data(), &ctx);
    return digest;
}

bool starts_with_five_zeroes(const Digest& d)
{
    return (d[0] == 0) && (d[1] == 0) && ((d[2] & 0xf0) == 0);
}

bool starts_with_six_zeroes(const Digest& d)
{
    return (d[0] == 0) && (d[1] == 0) && (d[2] == 0);
}

// Returns an md5 function with its own internal storage prefixed with the
// provided key, to which it appends the input integer.
auto md5generator(std::string_view key)
{
    std::array<char, 64> md5input{0};
    const char* key_end{r::copy(key, md5input.data()).out};
    const std::size_t number_index{
        static_cast<std::size_t>(key_end - md5input.data())};
    return [md5input, number_index](int i) mutable {
        char* number_end{
            fmt::format_to(md5input.data() + number_index, "{}", i)};
        return md5({md5input.data(), number_end});
    };
}

}  // namespace

aoc::solution_result day04(std::string_view input)
{
    input = trim(input);
    auto my_md5 = md5generator(input);
    int i{0};
    while (!starts_with_five_zeroes(my_md5(i))) {
        i++;
    }
    const auto a{i};
    while (!starts_with_six_zeroes(my_md5(i))) {
        i++;
    }
    const auto b{i};
    // TODO: Parallelize.

    return {a, b};
}

}  // namespace aoc::year2015
