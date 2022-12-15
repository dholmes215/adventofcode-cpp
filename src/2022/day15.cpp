//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <ctre.hpp>

#include <array>
#include <cstdint>
#include <optional>
#include <set>
#include <string_view>
#include <vector>

namespace aoc::year2022 {

namespace {

using int_t = int;
using pos_t = vec2<int_t>;
using rect_t = rect<int_t>;
using range_t = std::pair<int_t, int_t>;  // Bounds of a range, both inclusive

struct pair_t {
    pos_t sensor;
    pos_t beacon;
};

pair_t parse_line(std::string_view line)
{
    constexpr auto matcher{ctre::match<
        R"(Sensor at x=(.+), y=(.+): closest beacon is at x=(.+), y=(.+))">};
    if (auto [whole, x1, y1, x2, y2] = matcher(line); whole) {
        return {{to_num<int_t>(x1), to_num<int_t>(y1)},
                {to_num<int_t>(x2), to_num<int_t>(y2)}};
    }
    throw input_error{fmt::format("failed to parse input: {}", line)};
}

int_t manhattan(const pair_t& p)
{
    const auto delta{p.sensor - p.beacon};
    return std::abs(delta.x) + std::abs(delta.y);
}

bool within_diamond(const pair_t& pair, const pos_t pos)
{
    const auto& [sensor, beacon]{pair};
    return manhattan({sensor, pos}) <= manhattan(pair);
}

rect_t find_area(const std::vector<pair_t>& pairs)
{
    const auto greatest_manhattan{r::max(pairs | rv::transform(manhattan))};
    const auto positions{pairs | rv::transform([](const pair_t& p) {
                             return std::array<pos_t, 2>{p.sensor, p.beacon};
                         }) |
                         rv::join | r::to<std::vector>};
    pos_t top_left{r::front(positions)};
    pos_t bottom_right{r::front(positions)};
    for (const auto& p : positions) {
        top_left.x = r::min(top_left.x, p.x);
        top_left.y = r::min(top_left.y, p.y);
        bottom_right.x = r::max(bottom_right.x, p.x);
        bottom_right.y = r::max(bottom_right.y, p.y);
    }
    top_left -= {greatest_manhattan, greatest_manhattan};
    bottom_right += {greatest_manhattan, greatest_manhattan};
    return rect_from_corners(top_left, bottom_right);
}

std::optional<range_t> get_horizontal_range(const pair_t& pair, int_t row)
{
    const auto this_manhattan{manhattan(pair)};
    const auto& [sensor, beacon]{pair};
    const auto [x, y]{sensor};
    const int_t y_delta{std::abs(y - row)};
    const int_t half_range{this_manhattan - y_delta};
    if (half_range < 0) {
        return std::nullopt;
    }
    return range_t{x - half_range, x + half_range};
}

range_t combine_overlapping_ranges(const range_t& lhs, const range_t& rhs)
{
    if (rhs.first <= lhs.second + 1) {
        return range_t{lhs.first, std::max(lhs.second, rhs.second)};
    }
    return lhs;
}

range_t combine_many_overlapping_ranges(const std::vector<range_t>& ranges)
{
    return r::accumulate(ranges, ranges[0], combine_overlapping_ranges);
};

}  // namespace

aoc::solution_result day15(std::string_view input)
{
    const auto pairs{sv_lines(trim(input)) | rv::transform(parse_line) |
                     r::to<std::vector>};
    const auto beacon_set{
        pairs | rv::transform([](const pair_t p) { return p.beacon; }) |
        r::to<std::set>};
    const auto area{find_area(pairs)};

    const auto cannot_contain_beacon{[&](pos_t p) {
        const auto p_within_diamond{
            [p](const pair_t& pair) { return within_diamond(pair, p); }};
        return !beacon_set.contains(p) && r::any_of(pairs, p_within_diamond);
    }};

    const bool is_example_input{area.dimensions.x < 1000};
    const int_t part1_y{is_example_input ? 10 : 2000000};

    const auto row_positions{rv::iota(area.base.x) |
                             rv::take(area.dimensions.x) |
                             rv::transform([part1_y](int_t x) {
                                 return pos_t{x, part1_y};
                             })};

    const auto part1_count{r::count_if(row_positions, cannot_contain_beacon)};

    const int_t part2_max{is_example_input ? 20 : 4000000};

    const auto get_ranges{[&](const int_t y) -> const std::vector<range_t>& {
        const auto y_horizontal_range{
            [y](const pair_t& p) { return get_horizontal_range(p, y); }};
        static std::vector<range_t> ranges;
        ranges.clear();
        r::copy(
            pairs | rv::transform(y_horizontal_range) |
                rv::filter([](const auto& opt) { return opt.has_value(); }) |
                rv::transform([](const auto& opt) { return *opt; }),
            std::back_inserter(ranges));
        // TODO: Combined filter/transform view for std::optional
        r::sort(ranges);
        return ranges;
    }};
    const auto get_first_x_not_in_ranges{[&](const int_t y) {
        const auto range{combine_many_overlapping_ranges(get_ranges(y))};
        return range.second + 1;
    }};

    const auto search_rows{rv::iota(0) | rv::take(part2_max + 1)};
    const auto in_search_area{[=](auto yx_pair) {
        return static_cast<int_t>(yx_pair.second) <= part2_max;
    }};
    const auto [part2_y, part2_x]{
        (search_rows | rv::transform(get_first_x_not_in_ranges) |
         rv::enumerate | rv::filter(in_search_area) | r::to<std::vector>)
            .front()};
    const auto tuning_frequency{part2_x * 4000000LL +
                                static_cast<std::int64_t>(part2_y)};

    return {part1_count, tuning_frequency};
}

}  // namespace aoc::year2022
