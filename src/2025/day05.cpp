//
// Copyright (c) 2025 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <set>
#include <string_view>
#include <vector>

namespace aoc::year2025 {

namespace {

using ingredient_id = std::uint64_t;
using intervals_map = std::map<ingredient_id, bool>;

struct processed_input {
    intervals_map fresh_intervals;
    std::vector<ingredient_id> ingredients;
};

bool is_fresh(const intervals_map& fresh_intervals, const ingredient_id id)
{
    return std::prev(fresh_intervals.lower_bound(id + 1))->second;
}

processed_input process_input(std::string_view input)
{
    processed_input output;
    output.fresh_intervals[0] = false;

    const auto lines = sv_lines(input);
    auto iter = r::begin(lines);
    const auto end = r::end(lines);

    while (iter != end) {
        const auto line = *iter++;
        if (line == "") {
            break;
        }
        const auto dash = line.find('-');
        const auto left_inclusive = to_num<ingredient_id>(line.substr(0, dash));
        const auto right_inclusive =
            to_num<ingredient_id>(line.substr(dash + 1));

        const bool after_right_already_fresh =
            is_fresh(output.fresh_intervals, right_inclusive + 1);

        // Remove any existing entries in this interval
        auto next_in_interval =
            output.fresh_intervals.lower_bound(left_inclusive);
        while (next_in_interval != output.fresh_intervals.end() &&
               next_in_interval->first <= right_inclusive) {
            output.fresh_intervals.erase(next_in_interval++);
        }

        if (!is_fresh(output.fresh_intervals, left_inclusive)) {
            output.fresh_intervals[left_inclusive] = true;
        }
        if (!after_right_already_fresh) {
            output.fresh_intervals[right_inclusive + 1] = false;
        } else {
            output.fresh_intervals.erase(right_inclusive + 1);
        }
    }

    while (iter != end) {
        output.ingredients.push_back(to_num<ingredient_id>(*iter++));
    }

    return output;
}

std::uint64_t count_fresh(const intervals_map& fresh_intervals)
{
    std::uint64_t count = 0;
    auto interval_left = fresh_intervals.begin();
    auto interval_end = fresh_intervals.end();
    assert(interval_left != interval_end);
    if (interval_left->second == false) {
        ++interval_left;
    }

    while (interval_left != interval_end) {
        assert(interval_left->second == true);
        auto interval_right = std::next(interval_left);
        assert(interval_right != interval_end);
        assert(interval_right->second == false);
        count += interval_right->first - interval_left->first;
        interval_left = std::next(interval_right);
    }

    return count;
}

}  // namespace

aoc::solution_result day05(std::string_view input)
{
    input = trim(input);
    const auto [fresh_intervals, ingredients] = process_input(input);

    const auto is_fresh_predicate = [&fresh_intervals](const ingredient_id id) {
        return is_fresh(fresh_intervals, id);
    };
    const auto a = r::count_if(ingredients, is_fresh_predicate);
    const auto b = count_fresh(fresh_intervals);

    return {a, b};
}

}  // namespace aoc::year2025
