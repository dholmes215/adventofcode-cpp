//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <array>

#include <cstdint>
#include <deque>
#include <functional>
#include <string_view>
#include <vector>

namespace aoc::year2022 {

namespace {

using monkey_id_t = std::size_t;
using worry_t = std::uint64_t;

struct monkey_t {
    monkey_id_t id;
    std::deque<worry_t> items;
    std::function<worry_t(worry_t)> operation;
    worry_t test_div;
    monkey_id_t if_true;
    monkey_id_t if_false;
    std::uint64_t inspected_count{0};
};

template <typename Num>
Num extract_num(std::string_view line)
{
    auto rng{numbers<Num>(line)};
    return r::front(rng);
}

const std::function<worry_t(worry_t)> parse_operation(std::string_view line)
{
    std::array<std::string_view, 2> op_words;
    r::copy(sv_words(line) | rv::drop(4), op_words.data());
    char op{op_words[0][0]};
    switch (op) {
        case '*':
            if (op_words[1] == "old") {
                return [](worry_t worry) { return worry * worry; };
            }
            else {
                worry_t factor{to_num<worry_t>(op_words[1])};
                return [factor](worry_t worry) { return worry * factor; };
            }
        case '+': {
            worry_t addend{to_num<worry_t>(op_words[1])};
            return [addend](worry_t worry) { return worry + addend; };
        }
        default:
            throw input_error(fmt::format("Unsupported operation '{}'", op));
    }
}

const auto parse_monkey{[](const auto& lines_rng) {
    std::array<std::string_view, 7> lines;
    r::copy(lines_rng, lines.data());

    monkey_t out;
    out.id = extract_num<monkey_id_t>(lines[0]);
    out.items = numbers<worry_t>(lines[1]) | r::to<std::deque>;
    out.operation = parse_operation(lines[2]);
    out.test_div = extract_num<worry_t>(lines[3]);
    out.if_true = extract_num<monkey_id_t>(lines[4]);
    out.if_false = extract_num<monkey_id_t>(lines[5]);
    return out;
}};

void play_round(std::vector<monkey_t>& monkeys,
                worry_t worry_divisor,
                worry_t divisor_product)
{
    for (auto& monkey : monkeys) {
        while (!monkey.items.empty()) {
            monkey.inspected_count++;
            worry_t item{monkey.items.front()};
            monkey.items.pop_front();

            item = monkey.operation(item);
            item /= worry_divisor;
            item %= divisor_product;
            bool is_divisible{(item % monkey.test_div) == 0};
            monkey_id_t target{is_divisible ? monkey.if_true : monkey.if_false};
            monkeys[target].items.push_back(item);
        }
    }
}

std::uint64_t get_inspected_count(const monkey_t& m)
{
    return m.inspected_count;
}

worry_t get_test_divisor(const monkey_t& m)
{
    return m.test_div;
}

worry_t monkey_business(const std::vector<monkey_t>& monkeys)
{
    auto inspected{monkeys | rv::transform(get_inspected_count) |
                   r::to<std::vector>};
    r::partial_sort(inspected, inspected.begin() + 2, std::greater{});
    return inspected[0] * inspected[1];
}

}  // namespace

aoc::solution_result day11(std::string_view input)
{
    auto monkeys1{sv_lines(input) | rv::chunk(7) | rv::transform(parse_monkey) |
                  r::to<std::vector>};
    auto monkeys2{monkeys1};

    const worry_t divisor_product{
        r::accumulate(monkeys1 | rv::transform(get_test_divisor), worry_t{1},
                      std::multiplies{})};

    for (int i{0}; i < 20; i++) {
        play_round(monkeys1, 3, divisor_product);
    }

    for (int i{1}; i <= 10000; i++) {
        play_round(monkeys2, 1, divisor_product);
    }

    return {monkey_business(monkeys1), monkey_business(monkeys2)};
}

}  // namespace aoc::year2022
