//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <algorithm>
#include <span>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace aoc::year2023 {

namespace {

using int_t = std::int64_t;

struct rule_t {
    char var;
    char op;
    int_t value;
    std::string_view next;
};

rule_t parse_rule(std::string_view s)
{
    auto colon{s.find(':')};
    if (colon == s.npos) {
        return {'!', '!', 0, s};
    }
    rule_t out;
    out.var = s[0];
    out.op = s[1];
    out.value = to_num<int_t>(s.substr(2, colon - 2));
    out.next = s.substr(colon + 1);
    return out;
}

using workflow_t = std::pair<std::string_view, std::vector<rule_t>>;
using workflow_map_t =
    std::unordered_map<workflow_t::first_type, workflow_t::second_type>;

workflow_t parse_workflow(std::string_view line)
{
    workflow_t out;
    auto left{line.find('{')};
    auto right{line.size() - 1};
    out.first = line.substr(0, left);
    auto rules_sv{line.substr(left + 1, right - left - 1)};
    out.second = sv_split_range(rules_sv, ',') | rv::transform(parse_rule) |
                 r::to<std::vector>;
    return out;
}

struct part_t {
    int_t x;
    int_t m;
    int_t a;
    int_t s;

    int_t var(char c) const
    {
        switch (c) {
            case 'x':
                return x;
            case 'm':
                return m;
            case 'a':
                return a;
            case 's':
                return s;
        }
        throw input_error(fmt::format("invalid variable: {}", c));
    }
};

part_t parse_part(std::string_view line)
{
    auto n{numbers<int_t>(line)};
    auto iter{r::begin(n)};
    return {*iter++, *iter++, *iter++, *iter++};
}

bool evaluate_rule(const rule_t& rule, const part_t& part)
{
    if (rule.op == '!') {
        return true;
    }

    auto var{part.var(rule.var)};
    if (rule.op == '<') {
        return var < rule.value;
    }
    return var > rule.value;
}

std::string_view evaluate_rules(const std::span<const rule_t> rules,
                                const part_t& part)
{
    for (const rule_t& rule : rules) {
        if (evaluate_rule(rule, part)) {
            return rule.next;
        }
    }
    throw input_error("failed to evaluate rules");
}

bool accept_part(const workflow_map_t& workflows, const part_t& part)
{
    std::string_view current{"in"};
    while (true) {
        current = evaluate_rules(workflows.at(current), part);
        if (current == "A") {
            return true;
        }
        if (current == "R") {
            return false;
        }
    }
    throw input_error("failed to evaluate workflows");
}

int_t score_part(const part_t& part)
{
    return part.x + part.m + part.a + part.s;
}

std::vector<int_t> partition_variable_space(const workflow_map_t& workflows,
                                            char var)
{
    std::vector<int_t> out;
    out.push_back(1);
    for (const auto& rules : workflows | rv::values) {
        for (const rule_t& rule : rules) {
            if (rule.var == var) {
                if (rule.op == '<') {
                    out.push_back(rule.value);
                }
                else {
                    out.push_back(rule.value + 1);
                }
            }
        }
    }
    out.push_back(4001);
    r::sort(out);
    out.erase(r::unique(out), out.end());
    return out;
}

bool simplify_rules(std::vector<rule_t>& rules)
{
    bool out{false};
    while (rules.size() > 1) {
        if (rules.back().next == rules[rules.size() - 2].next) {
            rules[rules.size() - 2] = rules.back();
            rules.pop_back();
            out = true;
        }
        else {
            break;
        }
    }
    return out;
}

bool simplify_all_rules(workflow_map_t& workflows)
{
    bool out{false};
    for (auto& rules : workflows | rv::values) {
        out = simplify_rules(rules) || out;
    }
    return out;
}

void simplify_all_workflows(workflow_map_t& workflows)
{
    while (simplify_all_rules(workflows)) {
        std::map<std::string_view, std::string_view> to_replace;
        for (auto& [name, rules] : workflows) {
            if (rules.size() == 1 && name != "in") {
                to_replace[name] = rules[0].next;
            }
        }

        for (auto& [name, replacement] : to_replace) {
            for (auto& rules : workflows | rv::values) {
                for (rule_t& rule : rules) {
                if (rule.next == name) {
                    rule.next = replacement;
                }}
            }
            workflows.erase(name);
        }
    }
}

}  // namespace

aoc::solution_result day19(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    const auto split{r::find(lines, "")};
    const std::span<const std::string_view> workflow_lines(r::begin(lines),
                                                           split);
    const std::span<const std::string_view> part_lines(r::next(split),
                                                       r::end(lines));

    auto workflows{workflow_lines | rv::transform(parse_workflow) |
                   r::to<workflow_map_t>};
    simplify_all_workflows(workflows);

    const auto parts{part_lines | rv::transform(parse_part) |
                     r::to<std::vector>};

    const auto filter_func{
        [&](const part_t& part) { return accept_part(workflows, part); }};

    int_t part1{r::accumulate(
        parts | rv::filter(filter_func) | rv::transform(score_part), 0)};

    std::vector<int_t> partitioned_x{partition_variable_space(workflows, 'x')};
    std::vector<int_t> partitioned_m{partition_variable_space(workflows, 'm')};
    std::vector<int_t> partitioned_a{partition_variable_space(workflows, 'a')};
    std::vector<int_t> partitioned_s{partition_variable_space(workflows, 's')};

    int_t part2{0};
    for (auto [x, x_max] : partitioned_x | rv::sliding(1) | rv::drop_last(1)) {
        int_t x_len{*x_max - *x};

        for (auto [m, m_max] : partitioned_m | rv::sliding(1) | rv::drop_last(1)) {
            int_t m_len{*m_max - *m};

            for (auto [a, a_max] : partitioned_a | rv::sliding(1) | rv::drop_last(1)) {
                int_t a_len{*a_max - *a};

                for (auto [s, s_max] : partitioned_s | rv::sliding(1) | rv::drop_last(1)) {
                    int_t s_len{*s_max - *s};

                    part_t part{*x, *m, *a, *s};
                    if (accept_part(workflows, part)) {
                        part2 += x_len * m_len * a_len * s_len;
                    }
                }
            }
        }
    }

    return {part1, part2};
}

}  // namespace aoc::year2023
