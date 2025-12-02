//
// Copyright (c) 2025 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <string_view>
#include <vector>

namespace aoc::year2025 {

namespace {

bool is_invalid_str_a(const std::string_view id)
{
    const auto half = id.length() / 2;
    return id.substr(0, half) == id.substr(half);
}

bool is_invalid_num_a(const std::int64_t id)
{
    return is_invalid_str_a(std::to_string(id));
}

bool is_invalid_str_b(const std::string_view id)
{
    for (auto chunk_size = 0UZ; chunk_size <= id.length() / 2; chunk_size++) {
        auto chunks = id | rv::chunk(chunk_size);
        auto first = chunks.front();
        auto equals_first = [first](auto a) { return r::equal(a, first); };
        if (r::all_of(chunks, equals_first)) {
            return true;
        }
    }
    return false;
}

bool is_invalid_num_b(const std::int64_t id)
{
    return is_invalid_str_b(std::to_string(id));
}

}  // namespace

aoc::solution_result day02(std::string_view input)
{
    input = trim(input);
    auto lines = sv_lines(input) | rv::join | r::to<std::vector>;
    auto nums =
        sv_split_range(lines, ',') |
        rv::transform([](auto&& sv) { return sv_split_range(sv, '-'); }) |
        rv::join | rv::transform(to_num<std::int64_t>) | r::to<std::vector>;

    auto pairs = nums | rv::chunk(2) | r::to<std::vector>;
    auto pair_to_rng = [](auto&& pair_rng) {
        auto l = pair_rng.front();
        auto count = pair_rng.back() - l + 1;
        return rv::iota(l) | rv::take(count);
    };
    auto all_nums = pairs | rv::transform(pair_to_rng) | rv::join;

    auto a = r::accumulate(all_nums | rv::filter(is_invalid_num_a), 0LL);
    auto b = r::accumulate(all_nums | rv::filter(is_invalid_num_b), 0LL);

    return {a, b};
}

}  // namespace aoc::year2025
