//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_ENUM_HPP
#define AOC_ENUM_HPP

#include "aoc_range.hpp"

#include <fmt/format.h>

#include <string_view>

namespace aoc {

template <typename Enum, typename NameRange>
Enum to_enum(NameRange names, std::string_view name)
{
    auto found{r::find(names, name)};
    if (found == names.end()) {
        throw input_error{fmt::format("unrecognized enum value: {}", name)};
    }
    return static_cast<Enum>(found - names.begin());
}

template <typename Enum, typename NameRange>
std::string_view enum_name(NameRange names, Enum value)
{
    return names[static_cast<std::size_t>(value)];
}

}  // namespace aoc

#endif  // AOC_ENUM_HPP
