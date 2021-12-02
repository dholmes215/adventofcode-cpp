//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GATE_HPP
#define GATE_HPP

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace aoc::year2015::gates {

using wire = std::string;
using signal = std::uint16_t;
using input = std::variant<signal, wire>;

enum class gate_type {
    assign,
    and_,
    or_,
    not_,
    lshift,
    rshift,
};

gate_type gate_type_from_sv(std::string_view sv);

std::string_view gate_type_to_sv(gate_type g) noexcept;

struct gate_description {
    gate_type type;
    wire output;
    input input1;
    std::optional<input> input2;
    friend auto operator<=>(const gate_description& lhs,
                            const gate_description& rhs) noexcept = default;
};

gate_description gate_from_sv(std::string_view sv);

class circuit {
    friend circuit build_circuit(std::string_view);

   public:
    void set_signal(wire w, signal s);
    signal get_signal(wire w) const;
    void evaluate();
    void reset();

   private:
    std::unordered_map<wire, gate_description> gates_;
    std::unordered_map<wire, signal> computed_signals_;

    signal evaluate_input(const input& i);
    signal gate_op(gate_type type, signal i1, std::optional<signal> i2);
    signal evaluate_wire(const wire& w);
};

circuit build_circuit(std::string_view in);

}  // namespace aoc::year2015::gates

#endif  // GATE_HPP
