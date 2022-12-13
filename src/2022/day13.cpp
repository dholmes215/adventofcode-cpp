//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <compare>
#include <functional>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

namespace aoc::year2022 {

namespace {

struct expr_t;
using int_t = int;
using list_t = std::vector<expr_t>;

struct expr_t {
    std::variant<list_t, int_t> val;

    list_t to_list() const
    {
        if (const int_t* maybe_int = std::get_if<int_t>(&val)) {
            return {{*maybe_int}};
        }
        return std::get<list_t>(val);
    }
    friend std::weak_ordering operator<=>(const expr_t& lhs, const expr_t& rhs)
    {
        if (lhs.val.index() == rhs.val.index() &&
            std::holds_alternative<int_t>(lhs.val)) {
            return std::get<int_t>(lhs.val) <=> std::get<int_t>(rhs.val);
        }
        list_t a{lhs.to_list()};
        list_t b{rhs.to_list()};
        return a <=> b;
    }
    friend bool operator==(const expr_t& lhs, const expr_t& rhs) = default;
};

using pair_t = std::pair<list_t, list_t>;

expr_t parse_expr(std::string_view& chars)
{
    if (is_digit(chars[0])) {
        auto end{chars.find_first_not_of("1234567890")};
        int_t out{to_num<int_t>(chars.substr(0, end))};
        chars = chars.substr(end);
        return {out};
    }
    if (chars[0] == '[') {
        list_t out;
        chars = chars.substr(1);
        while (chars[0] != ']') {
            if (chars[0] == ',') {
                chars = chars.substr(1);
            }
            out.push_back(parse_expr(chars));
        }
        chars = chars.substr(1);

        return {out};
    }
    throw input_error{fmt::format("Unexpected character: '{}'", chars[0])};
}

list_t as_list(const expr_t& expr)
{
    return std::get<list_t>(expr.val);
}

}  // namespace

aoc::solution_result day13(std::string_view input)
{
    auto lists{sv_lines(trim(input)) |
               rv::filter([](auto sv) { return !sv.empty(); }) |
               rv::transform([](auto sv) { return as_list(parse_expr(sv)); }) |
               r::to<std::vector>};

    const auto pairs{lists | rv::chunk(2) | rv::transform([](const auto& rng) {
                         auto first{r::front(rng)};
                         auto second{*(r::next(rng.begin()))};
                         return pair_t{first, second};
                     }) |
                     r::to<std::vector>};

    const auto enumerated1{rv::zip(rv::iota(1), pairs)};

    const auto right_order{enumerated1 | rv::filter([](const auto& p) {
                               const auto& [i, pair]{p};
                               return pair.first <= pair.second;
                           }) |
                           r::to<std::vector>};
    const auto index_sum{r::accumulate(right_order | rv::keys, 0)};

    const list_t two{{list_t{{2}}}};
    const list_t six{{list_t{{6}}}};
    lists.push_back(two);
    lists.push_back(six);
    r::sort(lists);
    const auto enumerated2{rv::zip(rv::iota(1), lists)};
    const auto is_divider{[&](const auto& p) {
        const auto& [i, l]{p};
        return l == two || l == six;
    }};
    const auto decoder_key{r::accumulate(
        enumerated2 | rv::filter(is_divider) | rv::keys, 1, std::multiplies{})};

    return {index_sum, decoder_key};
}

}  // namespace aoc::year2022
