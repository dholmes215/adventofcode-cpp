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

}  // namespace

aoc::solution_result day06(std::string_view input)
{
    const std::vector<std::vector<int>> input_ints{
        sv_lines(trim(input)) | rv::transform([](auto line) {
            return numbers<int>(line) | r::to<std::vector>;
        }) |
        r::to<std::vector>};

    int part1{1};
    for (std::size_t race{0}; race < input_ints[0].size(); race++) {
        int time{input_ints[0][race]};
        int record_distance{input_ints[1][race]};
        int ways{0};
        for (int button_hold{0}; button_hold <= time; button_hold++) {
            int distance{button_hold * (time - button_hold)};
            int margin{distance - record_distance};
            if (margin > 0) {
                ways++;
            }
        }
        part1 *= ways;
    }

    std::int64_t time{to_num<std::int64_t>(input_ints[0] |
                    rv::transform([](int i) { return fmt::format("{}", i); }) |
                    rv::join | r::to<std::string>)};
    std::int64_t record_distance{to_num<std::int64_t>(input_ints[1] | rv::transform([](int i) {
                                   return fmt::format("{}", i);
                               }) |
                               rv::join | r::to<std::string>)};
    std::int64_t ways{0};
    for (int button_hold{0}; button_hold <= time; button_hold++) {
        std::int64_t distance{button_hold * (time - button_hold)};
        std::int64_t margin{distance - record_distance};
        if (margin > 0) {
            ways++;
        }
    }

    return {part1, ways};
}

}  // namespace aoc::year2023
