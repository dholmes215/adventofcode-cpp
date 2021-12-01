//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "gate.hpp"

#include <cstdlib>
#include <regex>

namespace aoc::year2015::gates {

gate_type gate_type_from_sv(const std::string_view& sv)
{
    const auto found{r::find(gate_type_sv, sv)};
    if (found == gate_type_sv.end()) {
        throw input_error{fmt::format("Unknown gate type: {}", sv)};
    }
    return static_cast<gate_type>(found - gate_type_sv.begin());
}

std::string_view gate_type_to_sv(gate_type g) noexcept
{
    return gate_type_sv[static_cast<std::size_t>(g)];
}

input input_from_sv(std::string_view sv)
{
    if (r::all_of(sv, is_digit)) {
        return static_cast<signal>(std::atoi(sv.data()));
    }
    else if (r::all_of(sv, is_letter)) {
        return wire{sv};
    }
    throw input_error{fmt::format("Invalid input: {}", sv)};
}

gate_description gate_from_sv(std::string_view sv)
{
    const std::regex input_pattern{"(\\w+) -> ([a-z]+)"};
    const std::regex not_pattern{"NOT (\\w+) -> ([a-z]+)"};
    const std::regex binary_pattern{"(\\w+) ([A-Z]+) (\\w+) -> ([a-z]+)"};

    std::smatch m;
    const std::string str{sv};
    if (std::regex_match(str, m, input_pattern)) {
        return {gate_type::assign, m[2].str(), input_from_sv(m[1].str()), {}};
    }
    else if (std::regex_match(str, m, not_pattern)) {
        return {gate_type::not_, m[2].str(), input_from_sv(m[1].str()), {}};
    }
    else if (std::regex_match(str, m, binary_pattern)) {
        return {gate_type_from_sv(m[2].str()), m[4].str(),
                input_from_sv(m[1].str()), input_from_sv(m[3].str())};
    }
    throw input_error{fmt::format("Invalid gate description format: {}", sv)};
}

void circuit::set_signal(wire w, signal s)
{
    computed_signals_.emplace(w, s);
}

signal circuit::get_signal(wire w) const
{
    return computed_signals_.at(w);
}

void circuit::evaluate()
{
    for (const auto& w : gates_ | rv::keys) {
        evaluate_wire(w);
    }
}

void circuit::reset()
{
    computed_signals_.clear();
}

signal circuit::evaluate_input(const input& i)
{
    switch (i.index()) {
        case 0:  // signal
            return std::get<signal>(i);
        case 1:  // wire
            return evaluate_wire(std::get<wire>(i));
    }
    std::abort();  // Unreachable
}

signal circuit::gate_op(gate_type type, signal i1, std::optional<signal> i2)
{
    switch (type) {
        case gate_type::assign:
            return i1;
        case gate_type::and_:
            return i1 & *i2;
        case gate_type::or_:
            return i1 | *i2;
        case gate_type::not_:
            return ~i1;
        case gate_type::lshift:
            return static_cast<signal>(i1 << *i2);
        case gate_type::rshift:
            return static_cast<signal>(i1 >> *i2);
    }
    std::abort();  // Unreachable
}

signal circuit::evaluate_wire(const wire& w)
{
    if (computed_signals_.count(w)) {
        return computed_signals_.at(w);
    }

    const auto& desc{gates_.at(w)};
    signal i1_signal{evaluate_input(desc.input1)};
    auto maybe_i2_signal{
        desc.input2 ? std::optional<signal>{evaluate_input(*desc.input2)}
                    : std::nullopt};

    signal s{gate_op(desc.type, i1_signal, maybe_i2_signal)};
    computed_signals_.emplace(w, s);
    return s;
}

circuit build_circuit(std::string_view in)
{
    in = trim(in);
    circuit c;
    for (const auto& desc : sv_lines(in) | rv::transform(gate_from_sv)) {
        c.gates_.emplace(desc.output, desc);
    }
    return c;
}

}  // namespace aoc::year2015::gates
