//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <gate.hpp>

namespace aoc::year2015 {

aoc::solution_result day07(std::string_view input)
{
    auto circuit{gates::build_circuit(input)};
    circuit.evaluate();
    const auto answer_a{circuit.get_signal("a")};
    circuit.reset();
    circuit.set_signal("b", answer_a);
    circuit.evaluate();
    const auto answer_b{circuit.get_signal("a")};
    return {answer_a, answer_b};
}

}  // namespace aoc::year2015
