//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <ctre.hpp>

#include <algorithm>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

std::vector<int> parse_winning_numbers_from_line(std::string_view line)
{
    constexpr auto matcher{ctre::match<R"(Card (.+): (.+) \| (.+))">};
    if (auto [whole, card, winning_sv, our_sv] = matcher(line); whole) {
        std::vector<int> winning{numbers<int>(winning_sv) | r::to<std::vector>};
        std::vector<int> our{numbers<int>(our_sv) | r::to<std::vector>};
        r::sort(winning);
        r::sort(our);
        std::vector<int> out;
        r::set_intersection(winning, our, std::back_inserter(out));
        return out;
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

}  // namespace

aoc::solution_result day04(std::string_view input)
{
    const std::vector<int> winning_number_counts{
        sv_lines(trim(input)) | rv::transform(parse_winning_numbers_from_line) |
        rv::transform([](const std::vector<int>& v) {
            return static_cast<int>(v.size());
        }) |
        r::to<std::vector>};

    const auto scores{winning_number_counts | rv::transform([](int s) {
                          if (s == 0) {
                              return 0;
                          }
                          return static_cast<int>(std::pow(2, s - 1));
                      })};

    const int part1{r::accumulate(scores, 0)};

    std::vector<int> card_counts(winning_number_counts.size(), 1);
    for (std::size_t i{0}; i < card_counts.size(); i++) {
        int card_stack_size{card_counts[i]};
        int wins{winning_number_counts[i]};
        for (std::size_t j{i+1}; j < card_counts.size() && j < (i+1+wins); j++) {
            card_counts[j] += card_stack_size;
        }
    }

    const int part2{r::accumulate(card_counts, 0)};

    return {part1, part2};
}

}  // namespace aoc::year2023
