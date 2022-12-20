//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <iterator>
#include <list>
#include <string_view>
#include <vector>

namespace aoc::year2022 {

namespace {

using int_t = std::int64_t;

std::vector<int_t> mix(const std::vector<int_t>& nums, int times)
{
    const auto size{static_cast<int>(nums.size())};
    auto num_list{nums | r::to<std::list>};

    using iter_t = std::list<int_t>::iterator;
    std::vector<iter_t> iter_vec;
    iter_vec.reserve(nums.size());
    for (auto iter{num_list.begin()}; iter != num_list.end(); iter++) {
        iter_vec.push_back(iter);
    }

    for (int i{0}; i < times; i++) {
        for (const iter_t iter : iter_vec) {
            const auto num{*iter};

            auto insertion_iter{std::next(iter)};

            std::list<int_t> temp;
            temp.splice(temp.begin(), num_list, iter);

            const auto pos{std::distance(num_list.begin(), insertion_iter)};
            auto new_pos{pos + num};
            if (new_pos < 0) {
                new_pos +=
                    (((std::abs(new_pos) / (size - 1)) + 1) * (size - 1));
            }
            new_pos %= (size - 1);

            insertion_iter = num_list.begin();
            std::advance(insertion_iter, new_pos);
            if (insertion_iter == num_list.begin()) {
                insertion_iter = num_list.end();
            }

            num_list.splice(insertion_iter, temp);
        }
    }

    return num_list | r::to<std::vector>;
}

}  // namespace

aoc::solution_result day20(std::string_view input)
{
    const auto nums{int_lines(input) |
                    rv::transform([](int i) { return int_t{i}; }) |
                    r::to<std::vector>};

    auto mixed1{mix(nums, 1)};

    const auto zero1{std::find(mixed1.begin(), mixed1.end(), 0)};
    std::rotate(mixed1.begin(), zero1, mixed1.end());
    const auto cycle1{mixed1 | rv::cycle};
    const auto coords1{cycle1[1000] + cycle1[2000] + cycle1[3000]};

    const auto nums2{nums |
                     rv::transform([](int_t n) { return n * 811589153; }) |
                     r::to<std::vector>};
    auto mixed2{mix(nums2, 10)};

    const auto zero2{std::find(mixed2.begin(), mixed2.end(), 0)};
    std::rotate(mixed2.begin(), zero2, mixed2.end());
    const auto cycle2{mixed2 | rv::cycle};
    const auto coords2{cycle2[1000] + cycle2[2000] + cycle2[3000]};

    return {coords1, coords2};
}

}  // namespace aoc::year2022
