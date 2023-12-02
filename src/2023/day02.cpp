//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <algorithm>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

struct color_counts {
    int r{0};
    int g{0};
    int b{0};
};

constexpr const color_counts actual_bag{12, 13, 14};

color_counts parse_handful(std::string_view handful)
{
    color_counts out;
    auto parts{sv_split_range(handful, ',')};
    for (auto part : parts) {
        part = trim(part);
        int count{to_int(part.substr(0, part.find(' ')))};
        if (part.ends_with("red")) {
            out.r = count;
        }
        else if (part.ends_with("green")) {
            out.g = count;
        }
        else if (part.ends_with("blue")) {
            out.b = count;
        }
        else {
            throw input_error(
                fmt::format("Could not parse handful: {}", handful));
        }
    }
    return out;
}

struct game {
    int id;
    std::vector<color_counts> counts;
};

game parse_game(std::string_view line)
{
    game out;

    const auto colon_index{line.find(':')};
    out.id = to_int(line.substr(0, colon_index).substr(5));

    auto handfuls = sv_split_range(line.substr(colon_index + 1), ';');
    for (auto handful : handfuls) {
        out.counts.push_back(parse_handful(trim(handful)));
    }

    return out;
}

bool check_handful(const color_counts& handful, const color_counts& bag)
{
    return handful.r <= bag.r && handful.g <= bag.g && handful.b <= bag.b;
}

bool check_game(const game& g, const color_counts& bag)
{
    return r::all_of(
        g.counts, [&](auto&& handful) { return check_handful(handful, bag); });
}

color_counts count_maxes(const game& g)
{
    color_counts out;
    for (const color_counts& c : g.counts) {
        out.r = std::max(out.r, c.r);
        out.g = std::max(out.g, c.g);
        out.b = std::max(out.b, c.b);
    }
    return out;
}

int power(const color_counts& c)
{
    return c.r * c.g * c.b;
}

int game_power(const game& g)
{
    return power(count_maxes(g));
}

}  // namespace

aoc::solution_result day02(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    const auto games{lines | rv::transform(parse_game) | r::to<std::vector>};

    const auto check_game_actual_bag{
        [](const game& g) { return check_game(g, actual_bag); }};

    const int part1{
        r::accumulate(games | rv::filter(check_game_actual_bag) |
                          rv::transform([](const game& g) { return g.id; }),
                      0)};

    const int part2{r::accumulate(games | rv::transform(game_power), 0)};

    return {part1, part2};
}

}  // namespace aoc::year2023
