//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>

#include <fmt/format.h>

#include <string>
#include <string_view>

namespace aoc::year2015 {

namespace {

[[nodiscard]] std::string rle_encode(const std::string& input)
{
    std::string out;
    const auto end{input.end()};
    auto iter{input.begin()};
    while (iter != end) {
        const char c{*iter};
        int length{0};
        while (iter != end && *iter == c) {
            iter++;
            length++;
        }
        out.push_back(static_cast<char>(length) + '0');
        out.push_back(c);
    }

    return out;
}

}  // namespace

aoc::solution_result day10(std::string_view input)
{
    input = trim(input);

    std::string input_str{input};
    for (int i{0}; i < 40; i++) {
        input_str = rle_encode(input_str);
    }
    const auto result_a{input_str.length()};

    for (int i{0}; i < 10; i++) {
        input_str = rle_encode(input_str);
    }
    const auto result_b{input_str.length()};

    return {result_a, result_b};
}

}  // namespace aoc::year2015
