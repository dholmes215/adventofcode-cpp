//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <ctre.hpp>

#include <cstdint>
#include <string_view>
#include <type_traits>

namespace aoc::year2015 {

namespace {

using distance_t = std::uint64_t;

struct location_pair {
    std::string_view source;
    std::string_view destination;
    friend auto operator<=>(const location_pair& lhs,
                            const location_pair& rhs) = default;
};

struct location_entry {
    location_pair pair;
    distance_t distance;
};

location_entry parse_entry(std::string_view line)
{
    constexpr auto matcher{ctre::match<R"((.+) to (.+) = (\d+))">};
    if (auto [whole, source, destination, distance] = matcher(line); whole) {
        return {{source, destination}, to_num<distance_t>(distance)};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

}  // namespace

// This function returns a range, iterating which will permute the input and
// provide a reference to that permuted input.  This version modifies the input
// in place; an alternative design would be for the range to own a copy of the
// data (in a vector) and/or return copies.
auto permutation_generator(std::vector<std::string_view>& input)
{
    struct permute_result {
        std::remove_reference<decltype(input)>::type* in;
        bool found;
    };

    bool first{true};
    auto generator{[&input, first]() mutable -> permute_result {
        if (first) {
            // On the first call, return the current state of the input without
            // modifying it.
            first = false;
            return {&input, true};
        }
        return {&input, std::next_permutation(input.begin(), input.end())};
    }};

    auto take_until_done{rv::generate(generator) |
                         rv::take_while([](const permute_result& result) {
                             return result.found;
                         })};

    return take_until_done |
           rv::transform([](const permute_result& result) -> decltype(input)& {
               return *(result.in);
           });
}

aoc::solution_result day09(std::string_view input)
{
    const auto lines{sv_lines(input)};
    const auto entries{lines | rv::transform(parse_entry) | r::to<std::vector>};

    const auto pair_distances{entries |
                              rv::transform([](const location_entry& entry) {
                                  auto pair{entry.pair};
                                  if (pair.source < pair.destination) {
                                      std::swap(pair.source, pair.destination);
                                  }
                                  return std::make_pair(pair, entry.distance);
                              }) |
                              r::to<std::map<location_pair, distance_t>>};

    const auto sources{entries | rv::transform([](const auto& entry) {
                           return entry.pair.source;
                       })};
    const auto destinations{entries | rv::transform([](const auto& entry) {
                                return entry.pair.destination;
                            })};
    auto locations{rv::concat(sources, destinations) | r::to<std::vector>};

    r::sort(locations);
    locations.erase(r::unique(locations), locations.end());

    const auto permutation_to_cost{[&pair_distances](const auto& permutation) {
        const auto sliding{permutation | rv::sliding(2)};
        const auto costs{sliding |
                         rv::transform([&pair_distances](const auto&& pair) {
                             location_pair p2{pair[0], pair[1]};
                             if (p2.source < p2.destination) {
                                 std::swap(p2.source, p2.destination);
                             }
                             return pair_distances.at(p2);
                         })};
        return r::accumulate(costs, 0);
    }};

    const auto [shortest, longest]{r::minmax(
        permutation_generator(locations) | rv::transform(permutation_to_cost))};

    return {shortest, longest};
}

}  // namespace aoc::year2015
