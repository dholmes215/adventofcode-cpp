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
#include <string_view>
#include <vector>

namespace aoc::year2015 {

namespace {

using name_t = std::string_view;
using seconds_t = std::int32_t;
using speed_t = std::int32_t;
using distance_t = std::int32_t;

constexpr seconds_t race_duration{2503};

struct record_t {
    name_t name;
    speed_t speed;
    seconds_t flying;
    seconds_t resting;
};

record_t parse_line(std::string_view line)
{
    constexpr auto matcher{ctre::match<
        R"((.+) can fly (\d+) km/s for (\d+) seconds, but then must rest for (\d+) seconds\.)">};
    if (auto [whole, name, speed, flying, resting] = matcher(line); whole) {
        return {name.to_view(), to_num<speed_t>(speed),
                to_num<seconds_t>(flying), to_num<seconds_t>(resting)};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

distance_t calculate_distance(const record_t& record, const seconds_t duration)
{
    const auto period{record.flying + record.resting};
    const int completed_segment_count{duration / period};
    return (completed_segment_count * record.flying +
            std::min(duration % period, record.flying)) *
           record.speed;
}

distance_t calculate_distance_race(const record_t& record)
{
    return calculate_distance(record, race_duration);
}

}  // namespace

aoc::solution_result day14(std::string_view input)
{
    const auto records{sv_lines(trim(input)) | rv::transform(parse_line) |
                       r::to<std::vector>};

    const distance_t winner1_distance{
        r::max(records | rv::transform(calculate_distance_race))};

    // Part 2
    std::map<std::string_view, int> scores;
    for (seconds_t sec : rv::iota(seconds_t{1}, race_duration)) {
        const auto distance_func{[sec](const record_t& record) {
            return calculate_distance(record, sec);
        }};
        const distance_t leader_distance{
            r::max(records | rv::transform(distance_func))};
        for (const auto& record : records) {
            const auto our_distance{distance_func(record)};
            if (our_distance == leader_distance) {
                scores[record.name]++;
            }
        };
    }

    const int winner2_points{r::max(scores | rv::values)};

    return {winner1_distance, winner2_points};
}

}  // namespace aoc::year2015
