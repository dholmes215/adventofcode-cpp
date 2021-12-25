//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

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

using int_t = int;
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
        argument_t b{is_letter(words[2][0]) ? parse_register(words[2])
                                            : to_num<int_t>(words[2])};
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

input_t parse_serial_number(std::string_view input)
{
    auto parse_digit{[](char c) {
        if (c < '1' || c > '9') {
            throw input_error(fmt::format("invalid digit {}", c));
        }
        return c - '0';
    }};
    return input | rv::transform(parse_digit) | r::to<std::vector>;
}

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
                    reg(inst.a) = reg(inst.a) + eval(inst.b);
                    break;
                case opcode_t::mul:
                    reg(inst.a) = reg(inst.a) * eval(inst.b);
                    break;
                case opcode_t::div: {
                    const auto denom{eval(inst.b)};
                    if (denom == 0) {
                        throw alu_fault("division by zero");
                    }
                    reg(inst.a) = reg(inst.a) / denom;
                    break;
                }
                case opcode_t::mod: {
                    const auto denom{eval(inst.b)};
                    if (denom == 0) {
                        throw alu_fault("division by zero");
                    }
                    reg(inst.a) = reg(inst.a) % denom;
                    break;
                }
                case opcode_t::eql:
                    reg(inst.a) = reg(inst.a) == eval(inst.b) ? 1 : 0;
                    break;
            }
        }

        (void)program;
        //
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

}  // namespace

auto declining_serial_numbers()
{
    std::uint64_t i_start{99999999999999};
    std::vector<int_t> vec_start{};
    vec_start.resize(14);
    auto generator{
        [i = i_start, vec = vec_start]() mutable -> std::vector<int_t>& {
            // Parse i into vector
            auto i2{i};
            for (int_t& v : vec | rv::reverse) {
                v = i2 % 10;
                i2 /= 10;
            }
            i--;
            return vec;
        }};
    return rv::generate(generator) | rv::filter([](const auto& vec) {
               return r::none_of(vec, [](int_t v) { return v == 0; });
           });
}

std::string sn_string(const input_t& sn)
{
    return sn |
           rv::transform([](int_t v) { return static_cast<char>(v + '0'); }) |
           r::to<std::string>;
}

int program_step(int z0, int input, int a, int b)
{
    int w{0};
    int x{0};
    int y{0};
    int z{z0};

    w = input;               // 01 inp w
    x *= 0;                  // 02 mul x 0
    x += z;                  // 03 add x z
    x %= 26;                 // 04 mod x 26
    z /= 1;                  // 05 div z 1
    x += a;                  // 06 add x A
    x = ((x == w) ? 1 : 0);  // 07 eql x w
    x = ((x == 0) ? 1 : 0);  // 08 eql x 0
    y *= 0;                  // 09 mul y 0
    y += 25;                 // 10 add y 25
    y *= x;                  // 11 mul y x
    y += 1;                  // 12 add y 1
    z *= y;                  // 13 mul z y
    y *= 0;                  // 14 mul y 0
    y += w;                  // 15 add y w
    y += b;                  // 16 add y B
    y *= x;                  // 17 mul y x
    z += y;                  // 18 add z y

    return z;
}

bool whole_program(const int* input_begin)
{
    int z{0};
    z = program_step(z, *input_begin++, 13, 14);
    z = program_step(z, *input_begin++, 12, 8);
    z = program_step(z, *input_begin++, 11, 5);
    z = program_step(z, *input_begin++, 0, 4);
    z = program_step(z, *input_begin++, 15, 10);
    z = program_step(z, *input_begin++, -13, 13);
    z = program_step(z, *input_begin++, 10, 16);
    z = program_step(z, *input_begin++, -9, 5);
    z = program_step(z, *input_begin++, 11, 6);
    z = program_step(z, *input_begin++, 13, 13);
    z = program_step(z, *input_begin++, -14, 6);
    z = program_step(z, *input_begin++, -3, 7);
    z = program_step(z, *input_begin++, -2, 13);
    z = program_step(z, *input_begin++, -14, 3);
    return z == 0;
}

aoc::solution_result day24(std::string_view input)
{
    const program_t program{parse_program(input)};
    const input_t sample_serial_number{parse_serial_number("13579246899999")};

    alu_t alu;
    alu.run_program(program, sample_serial_number);
    // const auto result = alu.reg('z');
    std::string result;

    for (const auto& serial_number : declining_serial_numbers()) {
        alu.run_program(program, serial_number);
        if (alu.reg('z') == 0) {
            result = sn_string(serial_number);
            fmt::print("{} -> 0\n", result);
            break;
        }
    }

    return {result, 0};
}

}  // namespace aoc::year2021
