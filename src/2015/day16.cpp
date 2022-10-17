//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <ctre.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <map>
#include <string_view>

namespace aoc::year2015 {
namespace {

using count_t = std::int8_t;
using sue_id_t = int;

enum class thing_t {
    children,
    cats,
    samoyeds,
    pomeranians,
    akitas,
    vizslas,
    goldfish,
    trees,
    cars,
    perfumes
};

using thing_map_t = std::map<thing_t, count_t>;

struct sue_record_t {
    sue_id_t id;
    thing_map_t things;
};

constexpr std::array<std::string_view, 10> names{
    "children", "cats",     "samoyeds", "pomeranians", "akitas",
    "vizslas",  "goldfish", "trees",    "cars",        "perfumes"};

thing_t thing_from_sv(std::string_view str)
{
    const auto found{r::find(names, str)};
    if (found == names.end()) {
        return static_cast<thing_t>(-1);
    }
    return static_cast<thing_t>(r::distance(names.begin(), found));
}

[[maybe_unused]] std::string_view thing_to_sv(thing_t thing)
{
    const auto id{static_cast<std::size_t>(thing)};
    if (id >= 0 && id < names.size()) {
        return names[static_cast<std::size_t>(thing)];
    }
    return "unknown";
}

sue_record_t parse_line(std::string_view line)
{
    constexpr auto matcher{
        ctre::match<R"(Sue (\d+): (.+): (\d+), (.+): (\d+), (.+): (\d+))">};
    if (auto [whole, sue, thing1, count1, thing2, count2, thing3, count3] =
            matcher(line);
        whole) {
        return {to_num<sue_id_t>(sue),
                {{thing_from_sv(thing1), to_num<count_t>(count1)},
                 {thing_from_sv(thing2), to_num<count_t>(count2)},
                 {thing_from_sv(thing3), to_num<count_t>(count3)}}};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

const std::map<thing_t, count_t> ticker_tape_things{
    {thing_t::children, 3},    {thing_t::cats, 7},   {thing_t::samoyeds, 2},
    {thing_t::pomeranians, 3}, {thing_t::akitas, 0}, {thing_t::vizslas, 0},
    {thing_t::goldfish, 5},    {thing_t::trees, 3},  {thing_t::cars, 2},
    {thing_t::perfumes, 1}};

bool check_sue_part1(const sue_record_t& record)
{
    for (const auto& [thing, count] : record.things) {
        const auto found{ticker_tape_things.find(thing)};
        if (found != ticker_tape_things.end()) {
            const auto& [_, found_count] = *found;
            if (count != found_count) {
                return false;
            }
        }
    }
    return true;
}

bool check_thing_part2(thing_t thing,
                       count_t ticker_count,
                       count_t remembered_count)
{
    switch (thing) {
        case thing_t::cats:
        case thing_t::trees:
            return remembered_count > ticker_count;
        case thing_t::pomeranians:
        case thing_t::goldfish:
            return remembered_count < ticker_count;
        default:
            return remembered_count == ticker_count;
    }
}

bool check_sue_part2(const sue_record_t& record)
{
    for (const auto& [thing, remembered_count] : record.things) {
        const auto found{ticker_tape_things.find(thing)};
        if (found != ticker_tape_things.end()) {
            const auto& [_, ticker_count] = *found;
            if (!check_thing_part2(thing, ticker_count, remembered_count)) {
                return false;
            }
        }
    }
    return true;
}

}  // namespace
}  // namespace aoc::year2015

// XXX formatter specialization must be defined outside our namespace.
// Deriving from std::string_view formatter is a good "easy" way to implement
// these for an enum.
template <>
struct fmt::formatter<aoc::year2015::thing_t> : formatter<std::string_view> {
    template <typename FormatContext>
    auto format(aoc::year2015::thing_t thing, FormatContext& ctx) const
    {
        return formatter<std::string_view>::format(
            aoc::year2015::thing_to_sv(thing), ctx);
    }
};

namespace aoc::year2015 {

aoc::solution_result day16(std::string_view input)
{
    const auto lines{sv_lines(trim(input))};
    const auto entries{lines | rv::transform(parse_line) | r::to<std::vector>};

    const auto found1{r::find_if(entries, check_sue_part1)};
    const auto found2{r::find_if(entries, check_sue_part2)};

    if (found1 == entries.end()) {
        throw solution_error("Failed to find matching Aunt Sue for Part 1");
    }
    if (found2 == entries.end()) {
        throw solution_error("Failed to find matching Aunt Sue for Part 2");
    }

    return {found1->id, found2->id};
}

}  // namespace aoc::year2015
