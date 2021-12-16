//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "bits.hpp"

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <stdexcept>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

aoc::solution_result day16(std::string_view input)
{
    const auto bits{hex_to_bit_vector(input)};
    auto bit_iter{bits.begin()};
    packet p{read_packet(bit_iter, bits.end())};
    return {p.version_sum(), p.value()};
}

}  // namespace aoc::year2021
