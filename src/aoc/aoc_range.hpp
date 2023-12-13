//
// Copyright (c) 2021-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_RANGE_HPP
#define AOC_RANGE_HPP

#include "aoc.hpp"

// XXX In order to avoid build failures, do not include any range-v3 headers
// directly in any file! Instead, include this header to ensure this warning is
// suppressed.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4242)
#endif
#include <range/v3/all.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <map>
#include <string_view>
#include <vector>

namespace aoc {

namespace r = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;

// Given a std::map<Key,Value>, return the subset of the map ranging from
// first_key to last_key.
template <typename Key, typename Value>
auto submap(const std::map<Key, Value>& map,
            const Key& first_key,
            const Key& last_key)
{
    return r::subrange(map.lower_bound(first_key), map.upper_bound(last_key));
}

// Given a std::multimap<Key,Value>, return a range of all the values matching
// the given key.
template <typename Key, typename Value>
auto multimap_value_range(const std::multimap<Key, Value>& map, const Key& key)
{
    const auto [begin, end]{map.equal_range(key)};
    return r::subrange(begin, end) | rv::values;
}

// Convert a char range to a std::string_view.
// FIXME if the range isn't actually contiguous, this will never work.
auto sv(auto&& rng)
{
    return std::string_view{&*rng.begin(),
                            static_cast<std::size_t>(r::distance(rng))};
}

// Split a range by a delimiter into a range of string_views.
// XXX In C++23, split_view can produce ranges that are directly convertable to
// string_views, making this much simpler.
auto sv_split_range(auto&& rng, char delim) noexcept
{
    return rng | rv::split(delim) |
           rv::transform([&](auto&& r) { return sv(r); });
}

// Split a range of characters into a range of string_views by line.
auto sv_lines(auto&& rng) noexcept
{
    return sv_split_range(rng, '\n');
}

auto sv_words(auto&& rng) noexcept
{
    return rng | rv::split_when(is_whitespace) |
           rv::transform([](auto&& r) { return sv(r); }) | rv::remove("");
}

// Split a range of characters into a range of ints by line
auto int_lines(auto&& rng) noexcept
{
    return sv_lines(rng) | rv::transform(to_int);
}

// Split a range of characters into a range of ints separated by any non-digit
// characters
// FIXME: Should only allow '-' at the beginning of a number, not anywhere
template <typename Number>
auto numbers(auto&& rng)
{
    auto is_not_digit = [](char c) { return !is_digit(c) && (c != '-'); };
    return rng | rv::split_when(is_not_digit) |
           rv::transform([](auto&& r) { return sv(r); }) | rv::remove("") |
           rv::remove("-") | rv::transform(to_num<Number>);
}

int bool_range_to_int(auto&& bits)
{
    auto append_bit{
        [](int acc, int bit) { return (acc << 1) | (bit ? 1 : 0); }};
    return r::accumulate(bits, 0, append_bit);
}

template <typename BitType>
inline auto bit_range(std::int64_t i, std::size_t count)
{
    auto generator{
        [=]() mutable { return static_cast<BitType>((i >> --count) & 1); }};
    return rv::generate_n(generator, count);
}

// This function returns a range, iterating which will permute the input and
// provide a reference to that permuted input.  This version modifies the input
// in place; an alternative design would be for the range to own a copy of the
// data (in a vector) and/or return copies.
inline auto permutation_generator(std::vector<std::string_view>& input)
{
    struct permute_result {
        std::remove_reference<decltype(input)>::type* in;
        bool found;
    };

    bool first{true};
    auto generator{[&input, first]() mutable -> permute_result {
        if (first) {
            // On the first call, return the current state of the input without
            // modifying it.
            first = false;
            return {&input, true};
        }
        return {&input, std::next_permutation(input.begin(), input.end())};
    }};

    auto take_until_done{rv::generate(generator) |
                         rv::take_while([](const permute_result& result) {
                             return result.found;
                         })};

    return take_until_done |
           rv::transform([](const permute_result& result) -> decltype(input)& {
               return *(result.in);
           });
}

}  // namespace aoc

#endif  // AOC_RANGE_HPP
