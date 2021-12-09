//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <array>
#include <cassert>
#include <map>
#include <set>
#include <string>

namespace aoc::year2021 {

namespace {

}  // namespace

// Exception thrown when a string is not a valid signal set
class not_signal_error : public std::domain_error {
   public:
    not_signal_error(std::string msg) : std::domain_error{std::move(msg)} {}
};

// Exception thrown when multiple signals are assumed to be just oen
class multiple_signal_error : public std::domain_error {
   public:
    multiple_signal_error(std::string msg) : std::domain_error{std::move(msg)}
    {
    }
};

std::string sorted_unique(std::string_view sv)
{
    std::string s(sv);
    std::sort(s.begin(), s.end());
    s.erase(std::unique(s.begin(), s.end()), s.end());
    return s;
}

constexpr bool valid_char(char c) noexcept
{
    return c >= 'a' && c <= 'g';
}

class signal_set {
   public:
    signal_set() = default;

    signal_set(std::string_view sv) : chars_(sorted_unique(sv))
    {
        // Must contain only letters from a to g
        // if (!r::all_of(sv, valid_char)) {
        if (!std::all_of(sv.begin(), sv.end(), valid_char)) {
            throw new not_signal_error(
                fmt::format("{} contains invalid characters", sv));
        }
    }
    signal_set(char c) : signal_set(std::string{c})
    {
        if (!valid_char(c)) {
            throw new not_signal_error(
                fmt::format("{} is an invalid character", c));
        }
    }
    signal_set(const signal_set& rhs) : chars_(rhs.chars_){};
    signal_set& operator=(const signal_set& rhs)
    {
        chars_ = rhs.chars_;
        return *this;
    }

    char to_signal() const
    {
        if (chars_.size() > 1) {
            throw new multiple_signal_error(
                fmt::format("{} is not a single character", chars_));
        }
        return chars_[0];
    }

    auto cbegin() const noexcept { return chars_.cbegin(); }
    auto cend() const noexcept { return chars_.cend(); }
    auto begin() const noexcept { return cbegin(); }
    auto end() const noexcept { return cend(); }

    auto size() const noexcept { return chars_.size(); }

    friend signal_set operator+(const signal_set& lhs, const signal_set& rhs)
    {
        return signal_set(lhs.chars_ + rhs.chars_);
    }

    friend signal_set operator+(const signal_set& lhs, char rhs)
    {
        return signal_set(lhs + signal_set{rhs});
    }

    friend signal_set operator+(char lhs, const signal_set& rhs)
    {
        return signal_set(signal_set{lhs} + rhs);
    }

    friend signal_set operator-(const signal_set& lhs, const signal_set& rhs)
    {
        std::string s(lhs.chars_);
        for (char c : rhs.chars_) {
            s.erase(std::remove(s.begin(), s.end(), c), s.end());
        }
        return {s};
    }

    friend auto operator<=>(const signal_set& lhs,
                            const signal_set& rhs) noexcept = default;

    std::string_view chars() const noexcept { return chars_; }

   private:
    std::string chars_;
};

const signal_set all_signals{std::string_view("abcdefg")};

signal_set to_signal_set(std::string_view sv)
{
    return {sv};
}

struct nine_and_g {
    signal_set nine;
    char g;
};

nine_and_g determine_nine_and_g(std::set<signal_set>& unidentified_signals,
                                signal_set four,
                                char a)
{
    // 9 = 4 + a + g
    // We don't know g but there will only be one match
    for (const auto& nine_candidate : unidentified_signals) {
        for (char y : all_signals) {
            if (nine_candidate == four + a + y) {
                nine_and_g out{nine_candidate, y};
                unidentified_signals.erase(out.nine);
                return out;
            }
        }
    }

    throw new std::runtime_error("Could not find nine and g");
}

struct five_and_six {
    signal_set five;
    signal_set six;
};

five_and_six determine_five_and_six(std::set<signal_set>& unidentified_signals,
                                    char e)
{
    // 5 and 6 are the pair that you can add e to and get the other
    for (const auto& five_candidate : unidentified_signals) {
        for (const auto& six_candidate : unidentified_signals) {
            if (five_candidate == five_candidate + e) {
                continue;
            }
            if (five_candidate + e == six_candidate) {
                five_and_six out{five_candidate, six_candidate};
                unidentified_signals.erase(out.five);
                unidentified_signals.erase(out.six);
                return out;
            }
        }
    }

    throw new std::runtime_error("Could not find five and six");
}

struct zero_and_d {
    signal_set zero;
    char d;
};

zero_and_d determine_zero_and_d(std::set<signal_set>& unidentified_signals,
                                signal_set eight_signals)
{
    for (const auto& zero_candidate : unidentified_signals) {
        for (char y : all_signals) {
            if (zero_candidate == eight_signals - y) {
                zero_and_d out{zero_candidate, y};
                unidentified_signals.erase(out.zero);
                return out;
            }
        }
    }

    throw new std::runtime_error("Could not find zero and d");
}

signal_set determine_two(std::set<signal_set>& unidentified_signals,
                         signal_set acde)
{
    for (const auto& two_candidate : unidentified_signals) {
        for (char y : all_signals) {
            if (two_candidate == acde + y) {
                auto out{two_candidate};
                unidentified_signals.erase(out);
                return out;
            }
        }
    }

    throw new std::runtime_error("Could not find two");
}

signal_set determine_three(std::set<signal_set>& unidentified_signals,
                           signal_set acdg)
{
    for (const auto& three_candidate : unidentified_signals) {
        for (char y : all_signals) {
            if (three_candidate == acdg + y) {
                auto out{three_candidate};
                unidentified_signals.erase(out);
                return out;
            }
        }
    }

    throw new std::runtime_error("Could not find three");
}

aoc::solution_result day08(std::string_view input)
{
    auto lines{sv_lines(input)};

    int part_a_sum{0};
    int part_b_sum{0};

    for (auto line : lines) {
        auto parts{sv_split_range(line, '|') | r::to<std::vector>};
        auto scrambled_digit_signals{sv_words(r::front(parts)) |
                                     rv::transform(to_signal_set) |
                                     r::to<std::vector>};
        auto displayed_digit_signals{sv_words(r::front(parts | rv::drop(1))) |
                                     rv::transform(to_signal_set) |
                                     r::to<std::vector>};

        std::map<int, signal_set> digit_to_signals;
        std::map<signal_set, int> digits;

        for (auto signals : scrambled_digit_signals) {
            switch (signals.size()) {
                case 2:
                    digits[signals] = 1;
                    digit_to_signals[1] = signals;
                    break;
                case 4:
                    digits[signals] = 4;
                    digit_to_signals[4] = signals;
                    break;
                case 3:
                    digits[signals] = 7;
                    digit_to_signals[7] = signals;
                    break;
                case 7:
                    digits[signals] = 8;
                    digit_to_signals[8] = signals;
                    break;
                default:
                    // shrug
                    break;
            }
        }

        for (auto signals : displayed_digit_signals) {
            if (digits.contains(signals)) {
                part_a_sum++;
            }
        }

        // Determine the individual segment translation
        // a = 7 - 1
        const signal_set one_signals{digit_to_signals[1]};
        const signal_set four_signals{digit_to_signals[4]};
        const signal_set seven_signals{digit_to_signals[7]};
        const signal_set eight_signals{digit_to_signals[8]};

        const char a{(seven_signals - one_signals).to_signal()};

        std::set<signal_set> unidentified_signals{
            scrambled_digit_signals.begin(), scrambled_digit_signals.end()};
        unidentified_signals.erase(one_signals);
        unidentified_signals.erase(four_signals);
        unidentified_signals.erase(seven_signals);
        unidentified_signals.erase(eight_signals);

        const auto [nine_signals, g]{
            determine_nine_and_g(unidentified_signals, four_signals, a)};

        // e = 8 - 9
        const char e = signal_set{eight_signals - nine_signals}.to_signal();

        const auto [five_signals, six_signals]{
            determine_five_and_six(unidentified_signals, e)};

        // c = 9 - 5
        const char c = signal_set{nine_signals - five_signals}.to_signal();

        const auto [zero_signals, d]{
            determine_zero_and_d(unidentified_signals, eight_signals)};

        const signal_set acde{signal_set{a} + c + d + e};
        const signal_set two_signals{determine_two(unidentified_signals, acde)};

        const signal_set acdg{signal_set{a} + c + d + g};
        const signal_set three_signals{
            determine_three(unidentified_signals, acdg)};

        if (!unidentified_signals.empty()) {
            throw new std::runtime_error(
                "We should have identified all signals but we didn't");
        }

        digits[zero_signals] = 0;
        digits[two_signals] = 2;
        digits[three_signals] = 3;
        digits[five_signals] = 5;
        digits[six_signals] = 6;
        digits[nine_signals] = 9;

        int output_value{0};

        for (auto signals : displayed_digit_signals) {
            output_value *= 10;
            output_value += digits.at(signals);
        }
        part_b_sum += output_value;
    }
    return {part_a_sum, part_b_sum};
}

}  // namespace aoc::year2021
