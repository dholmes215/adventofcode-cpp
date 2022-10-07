//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace aoc::year2015 {

namespace {

using password_t = std::array<char, 8>;
using password_number_t = std::uint64_t;

password_number_t password_to_number(auto&& password)
{
    return r::accumulate(
        password, password_number_t{}, [](password_number_t out, char c) {
            auto char_num{static_cast<password_number_t>(c - 'a')};
            out *= 26;
            out += char_num;
            return out;
        });
}

password_t number_to_password(password_number_t number)
{
    password_t out{0};
    for (char& c : out | rv::reverse) {
        c = static_cast<char>(number % 26) + 'a';
        number /= 26;
    }
    return out;
}

password_t sv_to_password(std::string_view input)
{
    password_t out;
    // TODO: Check input
    r::copy(input, out.begin());
    return out;
}

bool check_increasing_straight(password_t password)
{
    const auto windows{password | rv::sliding(3)};
    return r::any_of(windows, [](const auto window) {
        return (window[1] == window[0] + 1) && (window[2] == window[0] + 2);
    });
}

bool check_disallowed_letters(password_t password)
{
    return r::none_of(password, [](const char c) {
        return c == 'i' || c == 'o' || c == 'l';
    });
}

bool check_two_pairs(password_t password)
{
    int count{0};
    const auto stop_iter{std::prev(password.end())};
    for (auto iter{password.begin()}; iter < stop_iter; iter++) {
        if (*iter == *std::next(iter)) {
            count++;
            iter++;
        }
    }
    return count >= 2;
}

bool check_valid_password(password_t password)
{
    return check_increasing_straight(password) &&
           check_disallowed_letters(password) && check_two_pairs(password);
}

std::string password_to_string(password_t password)
{
    return password | r::to<std::string>;
}

password_t next_valid_password(password_t password)
{
    auto num{password_to_number(password)};
    do {
        num++;
        password = number_to_password(num);
    } while (!check_valid_password(password));
    return password;
}

}  // namespace

aoc::solution_result day11(std::string_view input)
{
    input = trim(input);
    const auto password{sv_to_password(input)};
    const auto next_password{next_valid_password(password)};
    const auto next_next_password{next_valid_password(next_password)};

    return {password_to_string(next_password),
            password_to_string(next_next_password)};
}

}  // namespace aoc::year2015
