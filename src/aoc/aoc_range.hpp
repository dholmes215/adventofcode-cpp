//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_RANGE_HPP
#define AOC_RANGE_HPP

#include <range/v3/all.hpp>

#include <map>
#include <string_view>

namespace aoc {

namespace r = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;

using ranges::accumulate;
using ranges::adjacent_difference;
using ranges::adjacent_find;
using ranges::any_of;
using ranges::copy;
using ranges::count;
using ranges::count_if;
using ranges::distance;
using ranges::equal;
using ranges::equal_to;
using ranges::find;
using ranges::find_if;
using ranges::find_if_not;
using ranges::max_element;
using ranges::min_element;
using ranges::search;
using ranges::sort;
using ranges::subrange;
using ranges::to;
using ranges::unique;
using ranges::views::concat;
using ranges::views::drop;
using ranges::views::enumerate;
using ranges::views::filter;
using ranges::views::iota;
using ranges::views::join;
using ranges::views::partial_sum;
using ranges::views::reverse;
using ranges::views::split;
using ranges::views::stride;
using ranges::views::take;
using ranges::views::take_while;
using ranges::views::transform;
using ranges::views::zip_with;

// Given a std::map<Key,Value>, return the subset of the map ranging from
// first_key to last_key.
template <typename Key, typename Value>
auto submap(const std::map<Key, Value>& map,
            const Key& first_key,
            const Key& last_key)
{
    return subrange(map.lower_bound(first_key), map.upper_bound(last_key));
}

// Convert a char range to a std::string_view.
// FIXME if the range isn't actually contiguous, this will never work.
auto sv(auto&& rng)
{
    return std::string_view{&*rng.begin(),
                            static_cast<std::size_t>(distance(rng))};
}

// Split a range by a delimiter into a range of string_views.
// XXX In C++23, split_view can produce ranges that are directly convertable to
// string_views, making this much simpler.
auto sv_split_range(auto&& rng, char delim)
{
    return rng | split(delim) | transform([&](auto&& rng) { return sv(rng); });
}

// Split a range of characters into a range of string_views by line.
auto sv_lines(auto&& rng)
{
    return sv_split_range(rng, '\n');
}

}  // namespace aoc

#endif  // AOC_RANGE_HPP
