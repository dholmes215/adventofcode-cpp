//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <term.hpp>

#include <fmt/format.h>

#include <array>
#include <cassert>
#include <map>
#include <set>
#include <string>

namespace aoc::year2021 {

namespace {

}  // namespace

/*

0   6
1   2 *
2   5
3   5
4   4 *
5   5
6   6
7   3 *
8   7 *
9   6

*/

// Exception thrown when a string is not a valid signal set
class not_signal_error : public std::domain_error {
   public:
    not_signal_error(std::string msg) : std::domain_error{std::move(msg)} {}
};

constexpr std::string to_signal_string(std::string_view sv)
{
    std::string s(sv);
    // r::sort(s);
    std::sort(s.begin(), s.end());

    // Must contain no duplicates
    // r::unique(s);
    if (std::unique(s.begin(), s.end()) != s.end()) {
        throw new not_signal_error(fmt::format("{} contains duplicates", sv));
    }

    return s;
}

constexpr std::string sorted_unique(std::string_view sv)
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
    constexpr signal_set(std::string_view sv) 
        : chars_(sorted_unique(sv))
    {
        // Must contain only letters from a to g
        if (!r::all_of(sv, valid_char)) {
            throw new not_signal_error(fmt::format("{} contains invalid characters", sv));
        }
    }
    constexpr signal_set(char c) 
        : signal_set(std::string{c})
    {
        if (!valid_char(c)) {
            throw new not_signal_error(fmt::format("{} is an invalid character", c));
        }
    }

    constexpr friend signal_set operator+(const signal_set& lhs, const signal_set& rhs)
    {
        return signal_set(lhs.chars_ + rhs.chars_);
    }

    constexpr std::string_view chars() const noexcept { return chars_; }

private:
    std::string chars_;
};



dh::color color;


void print_set(const std::set<std::string>& remaining) {
    fmt::print("unidentified: ");
    for (const auto& s : remaining) {
        fmt::print("{} ", s);
    }
    fmt::print("\n");
}

aoc::solution_result day08(std::string_view input)
{
    auto lines{sv_lines(input)};

    int part_a_sum{0};
    int part_b_sum{0};

    for (auto line : lines) {
        auto parts{sv_split_range(line, '|') | r::to<std::vector>};
        auto scrambled_digit_words{sv_words(r::front(parts)) |
                                   rv::transform(to_signal_string) |
                                   r::to<std::vector>};
        auto displayed_digit_words{sv_words(r::front(parts | rv::drop(1))) |
                                   rv::transform(to_signal_string) |
                                   r::to<std::vector>};

        std::map<std::string, int> signals_to_digit;
        std::map<int, std::string> digit_to_signals;

        // std::map<char, char> unscrambled_segments;

        for (auto signals : scrambled_digit_words) {
            switch (signals.size()) {
                case 2:
                    signals_to_digit[signals] = 1;
                    digit_to_signals[1] = signals;
                    break;
                case 4:
                    signals_to_digit[signals] = 4;
                    digit_to_signals[4] = signals;
                    break;
                case 3:
                    signals_to_digit[signals] = 7;
                    digit_to_signals[7] = signals;
                    break;
                case 7:
                    signals_to_digit[signals] = 8;
                    digit_to_signals[8] = signals;
                    break;
                default:
                    // shrug
                    break;
            } 
        }

        for (auto signals : displayed_digit_words) {
            if (signals_to_digit.contains(signals)) {
                part_a_sum++;
            }
            
        }
        
        // Determine the individual segment translation
        // a = 7 - 1
        const std::string one{digit_to_signals[1]};
        const std::string four{digit_to_signals[4]};
        const std::string seven{digit_to_signals[7]};
        const std::string eight{digit_to_signals[8]};
        std::vector<char> seven_minus_one{seven | r::to<std::vector>};
        std::erase(seven_minus_one, one[0]);
        std::erase(seven_minus_one, one[1]);
        
        const char a = seven_minus_one[0];
        

        std::set<std::string> unidentified_signals{scrambled_digit_words.begin(), scrambled_digit_words.end()};
        unidentified_signals.erase(unidentified_signals.find(one));
        unidentified_signals.erase(unidentified_signals.find(four));
        unidentified_signals.erase(unidentified_signals.find(seven));
        unidentified_signals.erase(unidentified_signals.find(eight));

        // 9 = 4 + a + g
        // We don't know g but there will only be one match
        std::string four_plus_a{four + a};
        r::sort(four_plus_a);
        
        std::string nine;
        char g{'\0'};
        for (const auto& x : unidentified_signals) {
            for (char y : std::string_view{"abcdefg"}) {
                auto nine_candidate{four_plus_a + y};
                r::sort(nine_candidate);
                if (x == nine_candidate) {
                    nine = nine_candidate;
                    g = y;
                    break;
                }
            }
        }
        
        if (nine.empty()) {
            throw new std::runtime_error("Could not find nine and g");
        }

        signals_to_digit[nine] = 9;
        digit_to_signals[9] = nine;
        
        unidentified_signals.erase(unidentified_signals.find(nine));

        // e = 8 - 9
        std::vector<char> eight_minus_nine{eight | r::to<std::vector>};
        std::erase_if(eight_minus_nine, [&](char c) { return nine.find(c) != std::string::npos; });
        const char e = eight_minus_nine[0];
        (void)e;

        // 5 and 6 are the pair that you can add e to and get the other
        std::string five;
        std::string six;
        for (const auto& x : unidentified_signals) {
            for (const auto& y : unidentified_signals) {
                auto five_candidate{x};
                auto six_candidate{y};
                five_candidate.push_back(e);
                r::sort(five_candidate);
                if (five_candidate == six_candidate) {
                    five = x;
                    six = y;
                    break;
                }
            }
        }
        if (five.empty() || six.empty()) {
            throw new std::runtime_error("Could not find the pair of signals that add e to the other");
        }
        signals_to_digit[five] = 5;
        digit_to_signals[5] = five;
        signals_to_digit[six] = 6;
        digit_to_signals[6] = six;
        unidentified_signals.erase(unidentified_signals.find(five));
        unidentified_signals.erase(unidentified_signals.find(six));

        // 0, 2 and 3 are left
        // we know a and e

        // c = 9 - 5
        std::vector<char> nine_minus_five{nine | r::to<std::vector>};
        std::erase_if(nine_minus_five, [&](char c) { return five.find(c) != std::string::npos; });
        const char c = nine_minus_five[0];

        // we know a, c, e

        // 0 = 8 - d, and it's the only one that'd 8 - a char
        char d{'\0'};
        std::string zero;
        for (const auto& x : unidentified_signals) {
            for (char y : std::string_view{"abcdefg"}) {
                std::vector<char> zero_candidate{eight.begin(), eight.end()};
                std::erase(zero_candidate, y);
                if (x == std::string_view{zero_candidate.begin(), zero_candidate.end()}) {
                    d = y;
                    zero = x;
                    break;
                }
            }
        }
        if (zero.empty()) {
            throw new std::runtime_error("Could not find zero and d");
        }
        signals_to_digit[zero] = 0;
        digit_to_signals[0] = zero;
        unidentified_signals.erase(unidentified_signals.find(zero));

        // 2 and 3 are left
        // we know a, c, d, e

        std::string acde;
        acde.push_back(a);
        acde.push_back(c);
        acde.push_back(d);
        acde.push_back(e);
        char g2{'\0'};
        std::string two;
        for (const auto& x : unidentified_signals) {
            for (char y : std::string_view{"abcdefg"}) {
                auto two_candidate{acde};
                two_candidate.push_back(y);
                r::sort(two_candidate);
                if (two_candidate == x) {
                    two = two_candidate;
                    g2 = y;
                    break;
                }
            }
        }

        assert(g == g2);
        (void)g2;

        if (two.empty()) {
            throw new std::runtime_error("Could not find two and g");
        }
        signals_to_digit[two] = 2;
        digit_to_signals[2] = two;
        unidentified_signals.erase(unidentified_signals.find(two));

        // 3 is left
        // we know a, c, d, e, g

        std::string acdg;
        acdg.push_back(a);
        acdg.push_back(c);
        acdg.push_back(d);
        acdg.push_back(g);
        char f;
        std::string three;
        for (const auto& x : unidentified_signals) {
            for (char y : std::string_view{"abcdefg"}) {
                auto three_candidate{acdg};
                three_candidate.push_back(y);
                r::sort(three_candidate);
                if (three_candidate == x) {
                    three = three_candidate;
                    f = y;
                    break;
                }
            }
        }

        if (three.empty()) {
            throw new std::runtime_error("Could not find three and f");
        }
        signals_to_digit[three] = 3;
        digit_to_signals[3] = three;
        unidentified_signals.erase(unidentified_signals.find(three));

        if (!unidentified_signals.empty()) {
            throw new std::runtime_error("We should have identified all signals but we didn't");
        }
        // we know a, c, d, e, f, g


        (void)f;


        // f = 3 - 2


        int output_value{0};

        for (auto signals : displayed_digit_words) {
            output_value *= 10;
            output_value += signals_to_digit[signals];
        }
        part_b_sum += output_value;

    }
    return {part_a_sum, part_b_sum};
}

}  // namespace aoc::year2021
