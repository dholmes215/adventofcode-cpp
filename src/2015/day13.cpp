//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <ctre.hpp>

#include <cstdint>
#include <map>
#include <set>
#include <string_view>

namespace aoc::year2015 {

namespace {

using name_t = std::string_view;
using happiness_t = std::int32_t;

struct neighbors {
    name_t self;
    name_t other;

    friend auto operator<=>(const neighbors&, const neighbors&) = default;
};

using entry_t = std::pair<neighbors, happiness_t>;
using neighbor_map_t = std::map<neighbors, happiness_t>;

entry_t parse_line(std::string_view line)
{
    constexpr auto matcher{ctre::match<
        R"((.+) would (gain|lose) (\d+) happiness units by sitting next to (.+)\.)">};
    if (auto [whole, self, gain_lose, amount, other] = matcher(line); whole) {
        const int sign{gain_lose.to_view() == "gain" ? 1 : -1};
        const happiness_t happiness{to_num<happiness_t>(amount) * sign};
        return {{self.to_view(), other.to_view()}, happiness};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

happiness_t permutation_score(const std::vector<name_t>& permutation,
                              const neighbor_map_t& scores)
{
    const auto pairs{permutation | rv::cycle |
                     rv::take(permutation.size() + 1) | rv::sliding(2)};
    const auto pair_cost{[&scores](decltype(r::front(pairs)) pair) {
        return scores.at({pair[0], pair[1]}) + scores.at({pair[1], pair[0]});
    }};
    return r::accumulate(pairs | rv::transform(pair_cost), 0);
}

}  // namespace

aoc::solution_result day13(std::string_view input)
{
    // XXX This solution simply exhaustively searches all permutations of the
    // seating arrangement.  It is not particularly fast but it is fast enough
    // and if there's a much smarter solution, it didn't jump out at me
    // immediately.  There are plenty of optimizations that could be done
    // though, like replacing the bidirectional map entries with a single entry
    // for a sorted pair that's the sum of the two, or even converting names to
    // ints and using a lookup table in a vector.

    const auto lines{sv_lines(trim(input))};
    neighbor_map_t neighbor_map{lines | rv::transform(parse_line) |
                                r::to<neighbor_map_t>};

    auto people{neighbor_map | rv::keys | rv::transform([](neighbors n) {
                    return std::array{n.self, n.other};
                }) |
                rv::join | r::to<std::set> | r::to<std::vector>};

    // Part 1
    const auto score{[&neighbor_map](const std::vector<name_t>& permutation) {
        return permutation_score(permutation, neighbor_map);
    }};
    const auto part1_max{
        r::max(permutation_generator(people) | rv::transform(score))};

    // Part 2
    // Add ourselves to the list of people and the map
    const std::string_view my_name{"Santa's Helper"};
    people.push_back("Santa's Helper");
    for (const auto [a, b] : people | rv::transform([&](auto name) {
                                 return neighbors{name, my_name};
                             })) {
        neighbor_map.insert({{a, b}, {}});
        neighbor_map.insert({{b, a}, {}});
    }
    const auto part2_max{
        r::max(permutation_generator(people) | rv::transform(score))};

    return {part1_max, part2_max};
}

}  // namespace aoc::year2015
