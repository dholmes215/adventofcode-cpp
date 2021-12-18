//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "snailfish.hpp"

#include <aoc.hpp>

#include <string_view>
#include <memory>
#include <variant>

namespace aoc::year2021 {

num_t copy_num_t(const num_t& num)
{
    if (std::holds_alternative<regular_num_t>(num)) {
        return std::get<regular_num_t>(num);
    }
    else {
        return std::make_unique<snail_num_t>(*std::get<snail_num_ptr_t>(num));
    }
}

bool num_t_equals(const num_t& lhs, const num_t& rhs)
{
    if (std::holds_alternative<regular_num_t>(lhs) &&
        std::holds_alternative<regular_num_t>(rhs)) {
        return std::get<regular_num_t>(lhs) == std::get<regular_num_t>(rhs);
    }
    else if (std::holds_alternative<snail_num_ptr_t>(lhs) &&
             std::holds_alternative<snail_num_ptr_t>(rhs)) {
        return *std::get<snail_num_ptr_t>(lhs) ==
               *std::get<snail_num_ptr_t>(rhs);
    }
    else {
        return false;
    }
}

regular_num_t magnitude(const num_t& num)
{
    if (std::holds_alternative<regular_num_t>(num)) {
        return std::get<regular_num_t>(num);
    }
    return std::get<snail_num_ptr_t>(num)->magnitude();
}

snail_num_t take_snail_num(num_t&& num)
{
    return *std::get<snail_num_ptr_t>(num);
}

num_t parse_num(const char*& iter, const char* const end)
{
    num_t out;
    if (*iter == '[') {
        // Snail number
        iter++;
        return std::make_unique<snail_num_t>(parse_snail_num(iter, end));
    }
    else {
        // Regular number
        auto num_begin{iter};
        iter = std::find_if(iter, end,
                            [](char c) { return c == ',' || c == ']'; });
        return num_t{to_num<regular_num_t>({num_begin, iter})};
    }
    return out;
}

snail_num_t parse_snail_num(const char*& iter, const char* const end)
{
    num_t left{parse_num(iter, end)};
    if (*iter++ != ',') {
        throw input_error("Expected ','");
    }
    num_t right{parse_num(iter, end)};
    if (*iter++ != ']') {
        throw input_error("Expected ']'");
    }
    return snail_num_t{std::move(left), std::move(right)};
}

snail_num_t parse_snail(std::string_view line)
{
    auto parse_begin{line.data()};
    return *std::get<snail_num_ptr_t>(
        parse_num(parse_begin, line.data() + line.size()));
}

num_t* snail_num_t::find_four_deep(int depth)
{
    if (std::holds_alternative<snail_num_ptr_t>(left_)) {
        if (depth == 3) {
            return &left_;
        }

        auto recurse =
            std::get<snail_num_ptr_t>(left_)->find_four_deep(depth + 1);
        if (recurse) {
            return recurse;
        }
    }

    if (std::holds_alternative<snail_num_ptr_t>(right_)) {
        if (depth == 3) {
            return &right_;
        }
        auto recurse =
            std::get<snail_num_ptr_t>(right_)->find_four_deep(depth + 1);
        if (recurse) {
            return recurse;
        }
    }

    return nullptr;
}

void snail_num_t::flatten_impl(std::vector<num_t*>& out)
{
    if (std::holds_alternative<regular_num_t>(left_)) {
        out.push_back(&left_);
    }
    else {
        std::get<snail_num_ptr_t>(left_)->flatten_impl(out);
    }

    if (std::holds_alternative<regular_num_t>(right_)) {
        out.push_back(&right_);
    }
    else {
        std::get<snail_num_ptr_t>(right_)->flatten_impl(out);
    }
}

std::vector<num_t*> snail_num_t::flatten()
{
    std::vector<num_t*> out;
    flatten_impl(out);
    return out;
}

void snail_num_t::explode(num_t* pair)
{
    auto flattened{flatten()};

    num_t* left{&std::get<snail_num_ptr_t>(*pair)->left_};
    auto left_iter{std::find(flattened.begin(), flattened.end(), left)};
    if (left_iter != flattened.begin()) {
        auto left_prev{std::prev(left_iter)};
        std::get<regular_num_t>(**left_prev) += std::get<regular_num_t>(*left);
    }

    num_t* right{&std::get<snail_num_ptr_t>(*pair)->right_};
    auto right_iter{std::find(flattened.begin(), flattened.end(), right)};
    if (right_iter != std::prev(flattened.end())) {
        auto right_next{std::next(right_iter)};
        auto right_val{std::get<regular_num_t>(*right)};
        std::get<regular_num_t>(**right_next) += right_val;
    }

    *pair = regular_num_t{0};
}

void snail_num_t::split_num(num_t& to_split)
{
    if (!std::holds_alternative<regular_num_t>(to_split)) {
        throw std::logic_error{
            "Tried to split a non-regular number; should be impossible"};
    }
    regular_num_t num = std::get<regular_num_t>(to_split);
    regular_num_t left = num / 2;
    regular_num_t right = num - left;
    to_split = std::make_unique<snail_num_t>(left, right);
}

// Returns true if a number was found and split
bool snail_num_t::split_ten_or_greater()
{
    if (std::holds_alternative<regular_num_t>(left_)) {
        if (std::get<regular_num_t>(left_) >= 10) {
            split_num(left_);
            return true;
        }
    }
    else if (std::get<snail_num_ptr_t>(left_)->split_ten_or_greater()) {
        return true;
    }

    if (std::holds_alternative<regular_num_t>(right_)) {
        if (std::get<regular_num_t>(right_) >= 10) {
            split_num(right_);
            return true;
        }
    }
    else if (std::get<snail_num_ptr_t>(right_)->split_ten_or_greater()) {
        return true;
    }

    return false;
}

// Returns true if any reduction was performed
bool snail_num_t::reduce_once()
{
    num_t* four_deep = find_four_deep();
    if (four_deep) {
        explode(four_deep);
        return true;
    }

    // if 10 or greater split
    if (split_ten_or_greater()) {
        return true;
    }

    return false;
}

void snail_num_t::reduce()
{
    while (reduce_once()) {
        // Do nothing
    }
}

}  // namespace aoc::year2021
