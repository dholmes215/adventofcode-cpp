//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <map>
#include <string_view>
#include <string>

namespace aoc::year2021 {

namespace {

using count_t = std::uint64_t;
using count_map_t = std::map<char, count_t>;
using pair_t = std::array<char, 2>;
using rule_t = std::pair<pair_t, char>;
using rules_t = std::map<pair_t, char>;

struct rule_application {
    rule_t rule;
    int iteration{0};
    friend auto operator<=>(const rule_application& lhs,
                            const rule_application& rhs) = default;
};

using memos_t = std::map<rule_application, count_map_t>;

count_map_t add_counts(const count_map_t& lhs, const count_map_t& rhs)
{
    count_map_t result = lhs;
    for (const auto& [key, value] : rhs) {
        result[key] += value;
    }
    return result;
}

std::pair<pair_t, char> parse_rule(std::string_view s)
{
    return {{s[0], s[1]}, s[6]};
}

count_map_t count_occurrences(std::string_view s)
{
    count_map_t counts;
    for (const auto c : s) {
        counts[c]++;
    }
    return counts;
}

auto count_min_max(const std::map<char, count_t>& counts)
{
    auto without_const{[](std::pair<char, count_t> p) {
        return std::pair<char, count_t>{p.first, p.second};
    }};
    return r::minmax(
        counts | rv::transform(without_const),
        [](const auto& a, const auto& b) { return a.second < b.second; });
}

count_map_t apply_rule(const rule_application& application,
                       const rules_t& rules,
                       memos_t& memos)
{
    if (application.iteration == 0) {
        return {};
    }
    if (memos.contains(application)) {
        return memos.at(application);
    }
    count_map_t result;
    if (application.iteration > 1) {
        pair_t ab{application.rule.first[0], application.rule.second};
        pair_t bc{application.rule.second, application.rule.first[1]};
        rule_t rule_ab{ab, rules.at(ab)};
        rule_t rule_bc{bc, rules.at(bc)};
        rule_application appl_ab{rule_ab, application.iteration - 1};
        rule_application appl_bc{rule_bc, application.iteration - 1};
        result = add_counts(apply_rule(appl_ab, rules, memos),
                            apply_rule(appl_bc, rules, memos));
    }
    result[application.rule.second]++;
    memos[application] = result;
    return result;
}

count_t solve(std::string_view polymer, const rules_t& rules, int iterations)
{
    memos_t memos;
    count_map_t counts{count_occurrences(polymer)};

    for (const auto p : polymer | rv::sliding(2)) {
        const pair_t a{p[0], p[1]};
        if (rules.contains(a)) {
            const rule_t rule{a, rules.at(a)};
            counts = add_counts(
                counts,
                apply_rule(rule_application{rule, iterations}, rules, memos));
        }
    }

    const auto [min, max]{count_min_max(counts)};
    const auto difference{max.second - min.second};
    return difference;
}

}  // namespace

aoc::solution_result day14(std::string_view input)
{
    const auto lines{sv_lines(input)};
    const std::string_view polymer_template{r::front(lines)};
    const rules_t insertion_rules{lines | rv::drop(2) |
                                  rv::transform(parse_rule) | r::to<std::map>};

    const auto result_10{solve(polymer_template, insertion_rules, 10)};
    const auto result_40{solve(polymer_template, insertion_rules, 40)};

    return {result_10, result_40};
}

}  // namespace aoc::year2021
