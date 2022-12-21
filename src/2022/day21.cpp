//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <map>
#include <optional>
#include <string_view>
#include <utility>

namespace aoc::year2022 {

namespace {

using int_t = double;

struct operation_t {
    char op;
    std::string_view lhs;
    std::string_view rhs;
};

struct monkey_t {
    std::string_view name;
    std::optional<int_t> value;
    std::optional<operation_t> operation;

    void simplify(std::map<std::string_view, monkey_t>& monkeys)
    {
        if (value) {
            return;
        }

        operation_t& o{*operation};
        auto find_lhs{monkeys.find(o.lhs)};
        auto find_rhs{monkeys.find(o.rhs)};
        if (find_lhs != monkeys.end()) {
            find_lhs->second.simplify(monkeys);
        }
        if (find_rhs != monkeys.end()) {
            find_rhs->second.simplify(monkeys);
        }
        if (find_lhs != monkeys.end() && find_rhs != monkeys.end() &&
            find_lhs->second.value && find_rhs->second.value) {
            int_t lhs{*(find_lhs->second.value)};
            int_t rhs{*(find_rhs->second.value)};
            switch (o.op) {
                case '+':
                    value = lhs + rhs;
                    break;
                case '-':
                    value = lhs - rhs;
                    break;
                case '*':
                    value = lhs * rhs;
                    break;
                case '/':
                    value = lhs / rhs;
                    break;
                case '=':
                    value = static_cast<int_t>(lhs == rhs);
                    break;
                default:
                    throw input_error(
                        fmt::format("Unsupported operation '{}'", o.op));
            }
        }
    }

    int_t evaluate_simplify(std::map<std::string_view, monkey_t>& monkeys)
    {
        simplify(monkeys);
        return *value;
    }

    int_t evaluate(const std::map<std::string_view, monkey_t>& monkeys) const
    {
        if (value) {
            return *value;
        }

        const operation_t& o{*operation};
        int_t lhs = monkeys.at(o.lhs).evaluate(monkeys);
        int_t rhs = monkeys.at(o.rhs).evaluate(monkeys);

        switch (o.op) {
            case '+':
                return lhs + rhs;
            case '-':
                return lhs - rhs;
            case '*':
                return lhs * rhs;
            case '/':
                return lhs / rhs;
            case '=':
                return static_cast<int_t>(lhs == rhs);
            default:
                throw input_error(
                    fmt::format("Unsupported operation '{}'", o.op));
        }
    }
};

std::pair<std::string_view, monkey_t> parse_monkey(std::string_view line)
{
    monkey_t monkey;
    monkey.name = line.substr(0, 4);
    if (line.size() >= 17) {
        monkey.operation =
            operation_t{line[11], line.substr(6, 4), line.substr(13, 4)};
    }
    else {
        monkey.value = static_cast<int_t>(to_int(line.substr(6)));
    }

    return std::pair{monkey.name, monkey};
}

}  // namespace

aoc::solution_result day21(std::string_view input)
{
    const std::map<std::string_view, monkey_t> input_monkeys{
        sv_lines(trim(input)) | rv::transform(parse_monkey) | r::to<std::map>};
    auto monkeys1{input_monkeys};
    const int_t part1{monkeys1.at("root").evaluate_simplify(monkeys1)};

    auto monkeys2{input_monkeys};
    monkey_t& root{monkeys2.at("root")};
    root.operation->op = '=';

    monkeys2.erase("humn");
    root.simplify(monkeys2);
    monkeys2.insert(*input_monkeys.find("humn"));

    monkey_t& humn{monkeys2.at("humn")};
    monkey_t& lhs{monkeys2.at(root.operation->lhs)};
    monkey_t& rhs{monkeys2.at(root.operation->rhs)};
    int_t rhs_value{*rhs.value};

    int_t part2{-1};
    int_t min{0};
    int_t max{100000000000000};
    const auto between{[&] { return (max + min) / 2; }};
    while (part2 == -1) {
        int_t i{between()};
        humn.value = i;
        int_t lhs_value{lhs.evaluate(monkeys2)};
        // fmt::print("{}: {} {}\n", i, lhs_value, rhs_value);
        if (lhs_value == rhs_value) {
            part2 = i;
        }
        // FIXME: This doesn't work for the example input; need to detect if
        // number is increasing or decreasing
        else if (lhs_value > rhs_value) {
            min = i;
        }
        else {
            max = i;
        }
    }

    return {part1, part2};
}

}  // namespace aoc::year2022
