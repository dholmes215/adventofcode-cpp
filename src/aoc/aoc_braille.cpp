//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "aoc_braille.hpp"

namespace aoc {

codepoint_t pack_bools(std::span<const bool, 8> bools)
{
    codepoint_t result{0};
    for (std::size_t i{0}; i < 8; ++i) {
        if (bools[i]) {
            result |= codepoint_t{1} << static_cast<codepoint_t>(i);
        }
    }
    return result;
}

codepoint_t to_braille(std::span<const bool, 4> bit_col_1,
                       std::span<const bool, 4> bit_col_2)
{
    std::array<bool, 8> bools{0};
    bools[0] = bit_col_1[0];
    bools[1] = bit_col_1[1];
    bools[2] = bit_col_1[2];
    bools[3] = bit_col_2[0];
    bools[4] = bit_col_2[1];
    bools[5] = bit_col_2[2];
    bools[6] = bit_col_1[3];
    bools[7] = bit_col_2[3];
    constexpr codepoint_t braille_start{0x2800};
    return braille_start + pack_bools(bools);
}

std::string to_braille_utf8(std::span<const bool, 4> bit_col_1,
                            std::span<const bool, 4> bit_col_2)
{
    return braille_codepoint_to_utf8(to_braille(bit_col_1, bit_col_2));
}

std::array<bool, 4> to_bit_col(bool a, bool b, bool c, bool d)
{
    return {a, b, c, d};
}

std::string braille_codepoint_to_utf8(codepoint_t codepoint)
{
    // Braille patterns are all 3 bytes in UTF-8, so the bits are placed in:
    // 1110xxxx 10xxxxxx 10xxxxxx
    std::array<char, 3> bytes{
        static_cast<char>(((codepoint >> 12) & 0b1111) | 0b11100000),
        static_cast<char>(((codepoint >> 6) & 0b111111) | 0b10000000),
        static_cast<char>(((codepoint >> 0) & 0b111111) | 0b10000000)};
    return {bytes.begin(), bytes.end()};
}

}  // namespace aoc
