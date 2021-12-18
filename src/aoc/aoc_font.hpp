//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_FONT_HPP
#define AOC_FONT_HPP

#include <span>
#include <stdexcept>

namespace aoc {

char recognize_char(std::span<const char, 30> data);
std::string recognize_string();  // TODO

// Exception thrown when input character data cannot be recognized.
class font_error : public std::domain_error {
   public:
    font_error(std::string msg) : domain_error{std::move(msg)} {}
};

}  // namespace aoc

#endif  // AOC_FONT_HPP
