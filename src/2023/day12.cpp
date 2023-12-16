//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_graph.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <algorithm>
#include <numeric>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

constexpr const int expansion_factor{5};

struct row_t {
    std::string records;
    std::vector<int> groups;
};

row_t parse_row(std::string_view line)
{
    return {std::string{line.substr(0, line.find(' '))},
            numbers<int>(line) | r::to<std::vector>};
}

row_t unfold_row(const row_t& r)
{
    return {rv::repeat(r.records) | rv::take(expansion_factor) | rv::join('?') |
                r::to<std::string>,
            rv::repeat(r.groups) | rv::take(expansion_factor) | rv::join |
                r::to<std::vector>};
}

constexpr int group_min_space_required(std::span<const int> groups)
{
    return std::accumulate(groups.begin(), groups.end(), 0) +
           static_cast<int>(groups.size()) - 1;
}

constexpr bool match_char(char record, char candidate)
{
    return record == '?' || record == candidate;
}

constexpr bool test_candidate(std::string_view records,
                              std::string_view candidate)
{
    for (std::size_t i{0}; i < records.size(); i++) {
        if (!match_char(records[i], candidate[i])) {
            return false;
        }
    }
    return true;
}

bool test_candidate_memo(std::string_view records, std::string_view candidate)
{
    static std::map<std::pair<std::string_view, std::string>, bool> memos;

    if (records.size() > 15) {
        return test_candidate(records, candidate);
    }

    auto p{std::make_pair(records, std::string{candidate})};
    if (memos.contains(p)) {
        return memos.at(p);
    }
    auto out{test_candidate(records, candidate)};
    memos[p] = out;
    return out;
}

static_assert(test_candidate(".#??", ".#.#"));
static_assert(!test_candidate(".#??", "##.#"));
static_assert(!test_candidate(".#??", "#..#"));
static_assert(!test_candidate("??.", ".##"));

// Given a subregion in a row of records consisting of only # and ?, count how
// many ways a single group can fit into that subregion.
/*constexpr*/ int count_single_group_fits_in_subregion(
    std::string_view subregion,
    int group)
{
    if (group > static_cast<int>(subregion.size())) {
        return 0;
    }

    static std::map<std::pair<std::string_view, int>, int> memos;
    if (memos.contains({subregion, group})) {
        auto memo{memos.at({subregion, group})};
        return memo;
    }

    int out{0};

    std::string candidate(subregion.size(), '.');
    std::fill(candidate.begin(), candidate.begin() + group, '#');
    for (std::size_t i{0}; i < subregion.size() - group + 1; i++) {
        if (test_candidate_memo(subregion, candidate)) {
            out++;
        }
        candidate[i] = '.';
        if (i + group < subregion.size()) {
            candidate[i + group] = '#';
        }
    }

    memos[{subregion, group}] = out;
    return out;
}

// static_assert(count_single_group_fits_in_subregion("??????????", 3) == 8);

// Given a subregion in a row of records consisting of only # and ?, count how
// many ways a given sequence of groups can fit into that subregion.
/*constexpr*/ std::int64_t count_fits_in_subregion(std::string_view subregion,
                                                   std::span<const int> groups)
{
    if (subregion.contains('.')) {
        throw input_error(
            fmt::format("\"{}\" is not a valid subregion", subregion));
    }
    if (groups.size() == 0) {
        if (subregion.contains('#')) {
            return 0;
        }
        return 1;
    }
    if (groups.size() == 1) {
        return count_single_group_fits_in_subregion(subregion, groups.front());
    }

    std::vector<int> vec;
    vec = groups | r::to<std::vector>;
    static std::map<std::pair<std::string, std::vector<int>>, std::int64_t>
        memos;
    if (memos.contains({std::string{subregion}, vec})) {
        return memos.at({std::string{subregion}, vec});
    }

    std::int64_t out{0};

    int groups_first{groups.front()};
    auto groups_rest{groups.subspan(1)};

    // May actually be -1 which is fine
    int rest_space_needed{group_min_space_required(groups_rest)};

    int max_first_space{
        static_cast<int>(subregion.size() - rest_space_needed - 1)};

    for (int i{groups_first}; i <= max_first_space; i++) {
        if (subregion[i] == '#') {
            // Cannot split on subregion[i] if a group must span it
            continue;
        }
        std::string first_candidate(i, '.');
        std::fill(first_candidate.end() - groups_first, first_candidate.end(),
                  '#');
        if (test_candidate_memo(subregion.substr(0, i), first_candidate)) {
            out +=
                count_fits_in_subregion(subregion.substr(i + 1), groups_rest);
        }
    }

    memos[{std::string{subregion}, vec}] = out;
    return out;
}

// static_assert(count_fits_in_subregion("##", std::array<int, 1>{2}) == 1);
// static_assert(count_fits_in_subregion("??", std::array<int, 1>{2}) == 1);
// static_assert(count_fits_in_subregion("???", std::array<int, 1>{2}) == 2);

std::vector<std::string_view> split_into_subregions(std::string_view records)
{
    return sv_split_range(records, '.') |
           rv::filter([](auto&& rng) { return !r::empty(rng); }) |
           r::to<std::vector>;
}

std::int64_t count_arrangements(std::span<const std::string_view> subregions,
                                std::span<const int> groups)
{
    std::vector<std::string_view> subregions_vec;
    subregions_vec = subregions | r::to<std::vector>;
    std::vector<int> groups_vec;
    groups_vec = groups | r::to<std::vector>;
    static std::map<std::pair<std::vector<std::string_view>, std::vector<int>>,
                    std::int64_t>
        memos;
    if (memos.contains({subregions_vec, groups_vec})) {
        return memos.at({subregions_vec, groups_vec});
    }

    if (subregions.empty() && groups.empty()) {
        return 1;
    }
    if (subregions.empty()) {
        return 0;
    }

    std::int64_t out{0};

    auto first_subregion{subregions.front()};
    auto subregions_rest{subregions.subspan(1)};

    for (std::size_t i{0}; i <= groups.size(); i++) {
        std::span<const int> group_set{groups.subspan(0, i)};
        std::span<const int> group_rest{groups.subspan(i)};

        std::int64_t count_set{
            count_fits_in_subregion(first_subregion, group_set)};

        std::int64_t count_rest{
            count_arrangements(subregions_rest, group_rest)};

        out += count_set * count_rest;
    }

    memos[{subregions_vec, groups_vec}] = out;

    return out;
}

std::int64_t count_arrangements_row(const row_t& row)
{
    auto out{
        count_arrangements(split_into_subregions(row.records), row.groups)};
    return out;
}

}  // namespace

aoc::solution_result day12(std::string_view input)
{
    input = trim(input);
    const std::vector<row_t> rows{
        sv_lines(trim(input)) | rv::transform(parse_row) | r::to<std::vector>};

    auto part1{r::accumulate(rows | rv::transform(count_arrangements_row), 0)};

    const std::vector<row_t> part2_rows{rows | rv::transform(unfold_row) |
                                        r::to<std::vector>};

    auto part2{r::accumulate(part2_rows | rv::transform(count_arrangements_row),
                             std::int64_t{})};

    return {part1, part2};
}

}  // namespace aoc::year2023
