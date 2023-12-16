//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <algorithm>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

enum type {
    high_card,
    one_pair,
    two_pair,
    three_of_a_kind,
    full_house,
    four_of_a_kind,
    five_of_a_kind
};

type part1_hand_type(std::string_view hand)
{
    std::map<char, int> counts;
    for (char c : hand) {
        counts[c]++;
    }
    if (counts.size() == 5) {
        return high_card;
    }
    else if (counts.size() == 4) {
        return one_pair;
    }
    else if (counts.size() == 1) {
        return five_of_a_kind;
    }
    else {
        std::string copy{hand};
        r::sort(copy);
        if (counts.size() == 2) {
            if (copy[0] == copy[3] || copy[1] == copy[4]) {
                return four_of_a_kind;
            }
            else {
                return full_house;
            }
        }
        if (copy[0] == copy[2] || copy[1] == copy[3] || copy[2] == copy[4]) {
            return three_of_a_kind;
        }
        else {
            return two_pair;
        }
    }
    throw input_error(fmt::format("impossible hand: {}", hand));
}

type part2_hand_type(std::string_view hand)
{
    std::string without_jokers{hand |
                               rv::filter([](char c) { return c != 'J'; }) |
                               r::to<std::string>};
    r::sort(without_jokers);
    std::map<char, int> counts;
    for (char c : without_jokers) {
        counts[c]++;
    }
    switch (without_jokers.size()) {
        case 5:
            return part1_hand_type(hand);
        case 4: {
            if (counts.size() == 1) {
                return five_of_a_kind;
            }
            else if (counts.size() == 2) {
                if (counts.begin()->second == 2) {
                    return full_house;
                }
                return four_of_a_kind;
            }
            else if (counts.size() == 3) {
                return three_of_a_kind;
            }
            return one_pair;
        }
        case 3: {
            if (counts.size() == 1) {
                return five_of_a_kind;
            }
            else if (counts.size() == 2) {
                return four_of_a_kind;
            }
            return three_of_a_kind;
        }
        case 2: {
            if (counts.size() == 1) {
                return five_of_a_kind;
            }
            return four_of_a_kind;
        }
        case 1:
            return five_of_a_kind;
        case 0:
            return five_of_a_kind;
    }
    throw input_error(fmt::format("impossible hand: {}", hand));
}

int part1_card_strength(char c)
{
    if (is_digit(c)) {
        return c - '0';
    };
    switch (c) {
        case 'T':
            return 10;
        case 'J':
            return 11;
        case 'Q':
            return 12;
        case 'K':
            return 13;
        case 'A':
            return 14;
    }
    throw input_error(fmt::format("illegal card: {}", c));
}

int part2_card_strength(char c)
{
    if (is_digit(c)) {
        return c - '0';
    };
    switch (c) {
        case 'T':
            return 10;
        case 'J':
            return 1;
        case 'Q':
            return 12;
        case 'K':
            return 13;
        case 'A':
            return 14;
    }
    throw input_error(fmt::format("illegal card: {}", c));
}

bool part1_compare_hands_by_card_strength(std::string_view hand_a,
                                          std::string_view hand_b)
{
    return r::lexicographical_compare(hand_a, hand_b, std::less<int>{},
                                      part1_card_strength, part1_card_strength);
}

bool part2_compare_hands_by_card_strength(std::string_view hand_a,
                                          std::string_view hand_b)
{
    return r::lexicographical_compare(hand_a, hand_b, std::less<int>{},
                                      part2_card_strength, part2_card_strength);
}

struct hand_bid {
    std::string_view hand;
    int bid;
};

bool part1_compare_hand_bid(const hand_bid& a, const hand_bid& b)
{
    auto type_a{part1_hand_type(a.hand)};
    auto type_b{part1_hand_type(b.hand)};
    if (type_a == type_b) {
        return part1_compare_hands_by_card_strength(a.hand, b.hand);
    }
    return type_a < type_b;
}

bool part2_compare_hand_bid(const hand_bid& a, const hand_bid& b)
{
    auto type_a{part2_hand_type(a.hand)};
    auto type_b{part2_hand_type(b.hand)};
    if (type_a == type_b) {
        return part2_compare_hands_by_card_strength(a.hand, b.hand);
    }
    return type_a < type_b;
}

hand_bid parse_line(std::string_view line)
{
    return {line.substr(0, 5), to_int(line.substr(6))};
}

}  // namespace

aoc::solution_result day07(std::string_view input)
{
    std::vector<hand_bid> hand_bids{
        sv_lines(trim(input)) | rv::transform(parse_line) | r::to<std::vector>};
    std::sort(hand_bids.begin(), hand_bids.end(), part1_compare_hand_bid);
    std::int64_t part1{0};
    for (int i{0}; i < static_cast<int>(hand_bids.size()); i++) {
        std::int64_t rank{i + 1};
        part1 += rank * hand_bids[i].bid;
    }
    std::sort(hand_bids.begin(), hand_bids.end(), part2_compare_hand_bid);
    std::int64_t part2{0};
    for (int i{0}; i < static_cast<int>(hand_bids.size()); i++) {
        std::int64_t rank{i + 1};
        part2 += rank * hand_bids[i].bid;
    }

    return {part1, part2};
}

}  // namespace aoc::year2023
