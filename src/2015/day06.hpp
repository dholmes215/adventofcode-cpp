//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef DAY06_HPP
#define DAY06_HPP

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>
#include <compare>

#include <fmt/format.h>

#include <bitset>
#include <regex>
#include <string_view>

namespace aoc::year2015::lights {

using Scalar = int;
using Vec2 = aoc::vec2<Scalar>;

enum class action {
    on,
    off,
    toggle,
};

struct instruction {
    action action;
    rect<int> region;
    friend auto operator<=>(const instruction& lhs,
                            const instruction& rhs) noexcept = default;
};

instruction string_to_instruction(std::string_view s);

class binary_light {
   public:
    void turn_on() noexcept { value_ = true; }
    void turn_off() noexcept { value_ = false; }
    void toggle() noexcept { value_ = !value_; }
    operator bool() const noexcept { return value_; }

   private:
    bool value_{false};
};

class dimmable_light {
    using value_type = int32_t;

   public:
    void turn_on() noexcept { value_++; }
    void turn_off() noexcept
    {
        value_ = static_cast<value_type>(std::max(value_ - 1, 0));
    }
    void toggle() noexcept { value_ += 2; }
    operator value_type() const noexcept { return value_; }

   private:
    value_type value_{0};
};

using binary_light_grid = heap_grid<binary_light, 1000, 1000>;
using dimmable_light_grid = heap_grid<dimmable_light, 1000, 1000>;

void do_action(auto& l, action a) noexcept
{
    switch (a) {
        case action::on:
            l.turn_on();
            return;
        case action::off:
            l.turn_off();
            return;
        case action::toggle:
            l.toggle();
            return;
    }
}

}  // namespace aoc::year2015::lights

#endif  // DAY06_HPP
