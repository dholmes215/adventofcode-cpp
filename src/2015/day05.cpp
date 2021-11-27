//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <string_view>

namespace aoc::year2015 {

namespace {

bool is_vowel(char c)
{
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

bool three_vowels(std::string_view s)
{
    return count_if(s, is_vowel) >= 3;
}

bool letter_twice_in_a_row(std::string_view s)
{
    return adjacent_find(s, equal_to{}) != s.end();
}

bool bad_pair(char c1, char c2)
{
    return (c1 == 'a' && c2 == 'b') || (c1 == 'c' && c2 == 'd') ||
           (c1 == 'p' && c2 == 'q') || (c1 == 'x' && c2 == 'y');
}

bool no_bad_substring(std::string_view s)
{
    return adjacent_find(s, bad_pair) == s.end();
}

bool nice_string_a(std::string_view s)
{
    return three_vowels(s) && letter_twice_in_a_row(s) && no_bad_substring(s);
}

bool non_overlapping_repeated_pair(std::string_view s)
{
    // XXX This is inelegant and inefficient, but the strings are short.
    for (auto iter = s.begin(); iter != s.end() - 2; ++iter) {
        std::string_view pair{iter, iter + 2};
        std::string_view rest{iter + 2, s.end()};
        if (search(rest, pair).begin() != rest.end()) {
            return true;
        }
    }
    return false;
}

bool repeat_with_letter_between(std::string_view s)
{
    for (std::size_t i = 0; i < s.size() - 2; ++i) {
        if (s[i] == s[i + 2]) {
            return true;
        }
    }
    return false;
}

bool nice_string_b(std::string_view s)
{
    return non_overlapping_repeated_pair(s) && repeat_with_letter_between(s);
}

}  // namespace

aoc::solution_result day05(std::string_view input)
{
    const auto nice_count_a{count_if(sv_lines(input), nice_string_a)};
    const auto nice_count_b{count_if(sv_lines(input), nice_string_b)};
    return {nice_count_a, nice_count_b};
}

}  // namespace aoc::year2015
