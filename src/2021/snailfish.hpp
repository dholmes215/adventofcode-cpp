//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SNAILFISH_HPP
#define SNAILFISH_HPP

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <string_view>
#include <memory>
#include <variant>

namespace aoc::year2021 {

using regular_num_t = int;
class snail_num_t;
using snail_num_ptr_t = std::unique_ptr<snail_num_t>;
using num_t = std::variant<regular_num_t, snail_num_ptr_t>;

num_t copy_num_t(const num_t& num);
bool num_t_equals(const num_t& lhs, const num_t& rhs);
regular_num_t magnitude(const num_t& num);

class snail_num_t {
    friend struct fmt::formatter<snail_num_t>;

   public:
    snail_num_t(const num_t& left, const num_t& right)
        : left_(copy_num_t(left)), right_(copy_num_t(right))
    {
    }

    snail_num_t(num_t&& left, num_t&& right)
        : left_{std::move(left)}, right_{std::move(right)}
    {
    }

    snail_num_t(const snail_num_t& other)
        : left_{copy_num_t(other.left_)}, right_{copy_num_t(other.right_)}
    {
    }

    snail_num_t(snail_num_t&& other)
        : left_{std::move(other.left_)}, right_{std::move(other.right_)}
    {
    }

    snail_num_t& operator=(const snail_num_t& other)
    {
        left_ = copy_num_t(other.left_);
        right_ = copy_num_t(other.right_);
        return *this;
    }

    snail_num_t& operator=(snail_num_t&& other)
    {
        left_ = std::move(other.left_);
        right_ = std::move(other.right_);
        return *this;
    }

    friend snail_num_t operator+(const snail_num_t& lhs, const snail_num_t& rhs)
    {
        snail_num_t out{std::make_unique<snail_num_t>(lhs),
                        std::make_unique<snail_num_t>(rhs)};
        out.reduce();
        return out;
    }

    friend snail_num_t operator+(const num_t& lhs, const num_t& rhs)
    {
        snail_num_t out{lhs, rhs};
        out.reduce();
        return out;
    }

    friend auto operator==(const snail_num_t& lhs, const snail_num_t& rhs)
    {
        return num_t_equals(lhs.left_, rhs.left_) &&
               num_t_equals(lhs.right_, rhs.right_);
    }

    regular_num_t magnitude()
    {
        return 3 * aoc::year2021::magnitude(left_) +
               2 * aoc::year2021::magnitude(right_);
    }

   private:
    num_t left_;
    num_t right_;

    num_t* find_four_deep(int depth = 0);

    void flatten_impl(std::vector<num_t*>& out);

    std::vector<num_t*> flatten();

    void explode(num_t* pair);

    void split_num(num_t& to_split);

    // Returns true if a number was found and split
    bool split_ten_or_greater();

    // Returns true if any reduction was performed
    bool reduce_once();

    void reduce();
};

snail_num_t take_snail_num(num_t&& num);

num_t parse_num(const char*& iter, const char* const end);

snail_num_t parse_snail_num(const char*& iter, const char* const end);

snail_num_t parse_snail(std::string_view line);

}  // namespace aoc::year2021

template <>
struct fmt::formatter<aoc::year2021::num_t> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        // No formatting options for this type.
        const auto it{ctx.begin()};
        if (it != ctx.end() && *it != '}') {
            throw format_error("invalid format");
        }
        return it;
    }

    template <typename FormatContext>
    auto format(const aoc::year2021::num_t& p, FormatContext& ctx)
        -> decltype(ctx.out())
    {
        if (std::holds_alternative<aoc::year2021::regular_num_t>(p)) {
            return format_to(ctx.out(), "{}",
                             std::get<aoc::year2021::regular_num_t>(p));
        }
        else {
            return format_to(
                ctx.out(), "{}",
                *std::get<std::unique_ptr<aoc::year2021::snail_num_t>>(p));
        }
    }
};

template <>
struct fmt::formatter<aoc::year2021::snail_num_t> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        // No formatting options for this type.
        const auto it{ctx.begin()};
        if (it != ctx.end() && *it != '}') {
            throw format_error("invalid format");
        }
        return it;
    }

    template <typename FormatContext>
    auto format(const aoc::year2021::snail_num_t& p, FormatContext& ctx)
        -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "[{},{}]", p.left_, p.right_);
    }
};

#endif  // SNAILFISH_HPP