//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace aoc::year2021 {

namespace {

enum class opcode_t {
    inp,
    add,
    mul,
    div,
    mod,
    eql,
};

constexpr opcode_t to_opcode(std::string_view s)
{
    if (s == "inp") {
        return opcode_t::inp;
    }
    else if (s == "add") {
        return opcode_t::add;
    }
    else if (s == "mul") {
        return opcode_t::mul;
    }
    else if (s == "div") {
        return opcode_t::div;
    }
    else if (s == "mod") {
        return opcode_t::mod;
    }
    else if (s == "eql") {
        return opcode_t::eql;
    }
    else {
        throw input_error(fmt::format("unknown opcode {}", s));
    }
}

constexpr std::string_view to_string(opcode_t opcode)
{
    switch (opcode) {
        case opcode_t::inp:
            return "inp";
        case opcode_t::add:
            return "add";
        case opcode_t::mul:
            return "mul";
        case opcode_t::div:
            return "div";
        case opcode_t::mod:
            return "mod";
        case opcode_t::eql:
            return "eql";
    }
    throw input_error(
        fmt::format("unknown opcode {}", static_cast<int>(opcode)));
}

using int_t = std::int64_t;
using register_id_t = char;
using argument_t = std::variant<int_t, register_id_t, std::nullopt_t>;

register_id_t parse_register(std::string_view s)
{
    if (s.size() != 1) {
        throw input_error(
            fmt::format("invalid register \"{}\" (size={})", s, s.size()));
    }
    if (s[0] < 'w' || s[0] > 'z') {
        throw input_error(
            fmt::format("invalid register \"{}\" (out of range)", s));
    }
    return s[0];
}

struct instruction {
    opcode_t opcode;
    register_id_t a;
    argument_t b;
};

// std::string to_string(const instruction& inst)
// {
//     std::string b;
//     if (std::holds_alternative<int_t>(inst.b)) {
//         b = fmt::format("{}", std::get<int_t>(inst.b));
//     }
//     else if (std::holds_alternative<register_id_t>(inst.b)) {
//         b = fmt::format("{}", std::get<register_id_t>(inst.b));
//     }  // else nullopt
//     return fmt::format("{} {} {}", to_string(inst.opcode), inst.a, b);
// }

instruction parse_instruction(std::string_view line)
{
    const auto words{sv_words(line) | r::to<std::vector>};
    const opcode_t opcode{to_opcode(words[0])};
    if (opcode == opcode_t::inp) {
        if (words.size() != 2) {
            throw input_error(
                fmt::format("incorrect number of parameters for {}: {}",
                            to_string(opcode), line));
        }
        return instruction{opcode, parse_register(words[1]), std::nullopt};
    }
    else {
        if (words.size() != 3) {
            throw input_error(
                fmt::format("incorrect number of parameters for `{} {} {}`",
                            words[0], words[1], words[2]));
        }

        argument_t b;
        if (is_letter(words[2][0])) {
            b.emplace<register_id_t>(parse_register(words[2]));
        }
        else {
            b.emplace<int_t>(to_num<int_t>(words[2]));
        }

        return instruction{opcode, parse_register(words[1]), b};
    }
}

using program_t = std::vector<instruction>;
using input_t = std::vector<int_t>;

program_t parse_program(std::string_view input)
{
    return sv_lines(input) | rv::transform(parse_instruction) |
           r::to<std::vector>;
}

struct step_parameters {
    int_t denom;  // 26 or 1; 1 always pushes; 26 must be made to only pop
    int_t a;
    int_t b;
};

std::vector<step_parameters> extract_step_parameters(const program_t& program)
{
    auto steps{program | rv::chunk(18)};
    std::vector<step_parameters> out;
    for (auto step : steps) {
        if (step.size() != 18) {
            throw input_error(fmt::format(
                "ran out of instructions ({}/18) in last step", step.size()));
        }

        step_parameters params;
        params.denom = std::get<int_t>(step[4].b);
        params.a = std::get<int_t>(step[5].b);
        params.b = std::get<int_t>(step[15].b);
        out.push_back(params);
    }

    return out;
}

struct largest_smallest {
    input_t largest;
    input_t smallest;
};

largest_smallest find_largest_and_smallest_sn(
    const std::vector<step_parameters>& params)
{
    std::vector<std::size_t> stack;
    std::vector<std::pair<std::size_t, std::size_t>> pairs;
    for (const auto& [i, step] : params | rv::enumerate) {
        if (step.denom == 1) {
            stack.push_back(i);
        }
        else {
            const std::size_t left{stack.back()};
            const std::size_t right{i};
            pairs.push_back({left, right});
            stack.pop_back();
        }
    }

    std::vector<int_t> largest;
    largest.resize(params.size());
    std::vector<int_t> smallest;
    smallest.resize(params.size());
    for (const auto& [left_idx, right_idx] : pairs) {
        int_t delta{params[left_idx].b + params[right_idx].a};
        auto& largest_left{largest[left_idx]};
        auto& largest_right{largest[right_idx]};
        auto& smallest_left{smallest[left_idx]};
        auto& smallest_right{smallest[right_idx]};
        if (delta > 0) {
            largest_left = 9 - delta;
            largest_right = 9;

            smallest_left = 1;
            smallest_right = 1 + delta;
        }
        else {
            largest_left = 9;
            largest_right = 9 + delta;

            smallest_left = 1 - delta;
            smallest_right = 1;
        }
    }

    return {largest, smallest};
}

// input_t parse_serial_number(std::string_view input)
// {
//     auto parse_digit{[](char c) -> int_t {
//         if (c < '1' || c > '9') {
//             throw input_error(fmt::format("invalid digit {}", c));
//         }
//         return c - '0';
//     }};
//     return input | rv::transform(parse_digit) | r::to<std::vector>;
// }

std::string sn_array_to_string(const input_t& sn_array)
{
    auto to_digit{[](int_t i) { return static_cast<char>('0' + i); }};
    return sn_array | rv::transform(to_digit) | r::to<std::string>;
}

// void print_stack(int_t stack)
// {
//     while (stack > 0) {
//         fmt::print("{} ", stack % 26);
//         stack /= 26;
//     }
//     fmt::print("\n");
// }

// Exception thrown when ALU executes something invalid
class alu_fault : public std::runtime_error {
   public:
    alu_fault(std::string msg) : runtime_error{std::move(msg)} {}
};

struct alu_t {
    std::array<int_t, 4> registers{};  // w, x, y, z

    void run_program(const program_t& program, const input_t& input)
    {
        registers = {0, 0, 0, 0};
        auto input_iter{input.begin()};

        for (instruction inst : program) {
            switch (inst.opcode) {
                case opcode_t::inp:
                    if (input_iter == input.end()) {
                        throw alu_fault(fmt::format(
                            "input exhausted before input instruction"));
                    }
                    reg(inst.a) = *input_iter++;
                    break;
                case opcode_t::add:
                    reg(inst.a) += eval(inst.b);
                    break;
                case opcode_t::mul:
                    reg(inst.a) *= eval(inst.b);
                    break;
                case opcode_t::div: {
                    const auto denom{eval(inst.b)};
                    if (denom == 0) {
                        throw alu_fault("division by zero");
                    }
                    reg(inst.a) /= denom;
                    break;
                }
                case opcode_t::mod: {
                    const auto denom{eval(inst.b)};
                    if (reg(inst.a) < 0) {
                        throw alu_fault("mod with negative numerator");
                    }
                    if (denom <= 0) {
                        throw alu_fault("mod by zero");
                    }
                    reg(inst.a) %= denom;
                    break;
                }
                case opcode_t::eql:
                    reg(inst.a) = (reg(inst.a) == eval(inst.b)) ? 1 : 0;
                    break;
            }
            // fmt::print("{:15} {:10} {:10} {:10} {:20}\n", to_string(inst),
            //            reg('w'), reg('x'), reg('y'), reg('z'));
        }
        if (input_iter != input.end()) {
            throw alu_fault(fmt::format("program did not read entire input"));
        }
    }

    int_t eval(argument_t arg) const
    {
        if (std::holds_alternative<int_t>(arg)) {
            return std::get<int_t>(arg);
        }
        else if (std::holds_alternative<register_id_t>(arg)) {
            return reg(std::get<register_id_t>(arg));
        }
        throw alu_fault("cannot evaluate nullopt argument");
    }

    int_t& reg(char c)
    {
        switch (c) {
            case 'w':
                return registers[0];
            case 'x':
                return registers[1];
            case 'y':
                return registers[2];
            case 'z':
                return registers[3];
            default:
                throw input_error(fmt::format("invalid register '{}'", c));
        }
    }

    int_t reg(char c) const
    {
        switch (c) {
            case 'w':
                return registers[0];
            case 'x':
                return registers[1];
            case 'y':
                return registers[2];
            case 'z':
                return registers[3];
            default:
                throw input_error(fmt::format("invalid register '{}'", c));
        }
    }
};

// auto declining_serial_numbers()
// {
//     std::uint64_t i_start{99999999999999};
//     std::vector<int_t> vec_start{};
//     vec_start.resize(14);
//     auto generator{
//         [i = i_start, vec = vec_start]() mutable -> std::vector<int_t>& {
//             // Parse i into vector
//             auto i2{i};
//             for (int_t& v : vec | rv::reverse) {
//                 v = i2 % 10;
//                 i2 /= 10;
//             }
//             i--;
//             return vec;
//         }};
//     return rv::generate(generator) | rv::filter([](const auto& vec) {
//                return r::none_of(vec, [](int_t v) { return v == 0; });
//            });
// }

// constexpr int_t program_step(const int_t z0,
//                              const int_t input,
//                              const int_t a,
//                              const int_t b,
//                              const int_t c)
// {
//     int_t w{0};
//     int_t x{0};
//     int_t y{0};
//     int_t z{z0};

//     w = input;               // 01 inp w
//     x *= 0;                  // 02 mul x 0
//     x += z;                  // 03 add x z
//     x %= 26;                 // 04 mod x 26
//     z /= a;                  // 05 div z A
//     x += b;                  // 06 add x B
//     x = ((x == w) ? 1 : 0);  // 07 eql x w
//     x = ((x == 0) ? 1 : 0);  // 08 eql x 0
//     y *= 0;                  // 09 mul y 0
//     y += 25;                 // 10 add y 25
//     y *= x;                  // 11 mul y x
//     y += 1;                  // 12 add y 1
//     z *= y;                  // 13 mul z y
//     y *= 0;                  // 14 mul y 0
//     y += w;                  // 15 add y w
//     y += c;                  // 16 add y C
//     y *= x;                  // 17 mul y x
//     z += y;                  // 18 add z y

//     print_stack(z);

//     return z;
// }

// bool whole_program(const int_t* input_begin)
// {
//     int_t z{0};
//     z = program_step(z, *input_begin++, 1, 13, 14);    // push 1
//     z = program_step(z, *input_begin++, 1, 12, 8);     // push 2
//     z = program_step(z, *input_begin++, 1, 11, 5);     // push 3
//     z = program_step(z, *input_begin++, 26, 0, 4);     // pop 3: 5
//     z = program_step(z, *input_begin++, 1, 15, 10);    // push 5
//     z = program_step(z, *input_begin++, 26, -13, 13);  // pop 5: -3
//     z = program_step(z, *input_begin++, 1, 10, 16);    // push 7
//     z = program_step(z, *input_begin++, 26, -9, 5);    // pop 7: 7
//     z = program_step(z, *input_begin++, 1, 11, 6);     // push 9
//     z = program_step(z, *input_begin++, 1, 13, 13);    // push 10
//     z = program_step(z, *input_begin++, 26, -14, 6);   // pop 10: -1
//     z = program_step(z, *input_begin++, 26, -3, 7);    // pop 9: 3
//     z = program_step(z, *input_begin++, 26, -2, 13);   // pop 2: 6
//     z = program_step(z, *input_begin++, 26, -14, 3);   // pop 1: 0
//     return z == 0;
// }

// bool test(const program_t& /*program*/, std::string_view sn)
// {
//     const input_t input{parse_serial_number(sn)};
//     // alu_t alu;
//     // alu.run_program(program, input);
//     // return alu.reg('z') == 0;
//     return whole_program(&*(input.begin()));
// }

}  // namespace

aoc::solution_result day24(std::string_view input)
{
    const program_t program{parse_program(input)};

    const auto [largest, smallest]{
        find_largest_and_smallest_sn(extract_step_parameters(program))};
    const auto largest_string{sn_array_to_string(largest)};
    const auto smallest_string{sn_array_to_string(smallest)};

    alu_t alu;
    alu.run_program(program, largest);
    if (alu.reg('z') != 0) {
        throw input_error(fmt::format(
            "largest serial number {} does not work with input program",
            largest_string));
    }

    alu.run_program(program, smallest);
    if (alu.reg('z') != 0) {
        throw input_error(fmt::format(
            "smallest serial number {} does not work with input program",
            smallest_string));
    }

    return {largest_string, smallest_string};
}

}  // namespace aoc::year2021
