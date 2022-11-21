//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <string_view>

namespace aoc::year2015 {

namespace {

// XXX The instructions say a register can hold "any non-negative integer" but
// I'm going to cross my fingers that uint64_t is good enough.
using word = std::uint64_t;
using instruction = std::string_view;

enum class reg_name { a, b };
reg_name reg_name_from_char(char c)
{
    switch (c) {
        case 'a':
            return reg_name::a;
        case 'b':
            return reg_name::b;
        default:
            throw input_error(fmt::format("Invalid register name {}", c));
    }
}

struct cpu {
    cpu(const std::vector<instruction> program) : program_{program} {}

    std::array<word, 2> registers{};
    int inst_ptr_{0};
    std::vector<instruction> program_;

    word& reg(reg_name r) { return registers[static_cast<std::size_t>(r)]; }
    reg_name to_reg(char c) { return reg_name_from_char(c); }

    void run()
    {
        while (inst_ptr_ >= 0 &&
               inst_ptr_ < static_cast<std::int64_t>(program_.size())) {
            run_instruction();
        }
    }

    void run_instruction()
    {
        const auto i{program_[static_cast<std::size_t>(inst_ptr_)]};
        auto parse_offset{[&](std::size_t offset) {
            int sign{i[offset] == '+' ? 1 : -1};
            return sign * to_int(i.substr(offset + 1));
        }};

        if (i.starts_with("hlf")) {
            inst_hlf(to_reg(i[4]));
        }
        else if (i.starts_with("tpl")) {
            inst_tpl(to_reg(i[4]));
        }
        else if (i.starts_with("inc")) {
            inst_inc(to_reg(i[4]));
        }
        else if (i.starts_with("jmp")) {
            inst_jmp(parse_offset(4));
        }
        else if (i.starts_with("jie")) {
            inst_jie(to_reg(i[4]), parse_offset(7));
        }
        else if (i.starts_with("jio")) {
            inst_jio(to_reg(i[4]), parse_offset(7));
        }
    }

    void inst_hlf(reg_name r)
    {
        reg(r) /= 2;
        inst_ptr_++;
    }

    void inst_tpl(reg_name r)
    {
        reg(r) *= 3;
        inst_ptr_++;
    }

    void inst_inc(reg_name r)
    {
        reg(r)++;
        inst_ptr_++;
    }

    void inst_jmp(int offset) { inst_ptr_ += offset; }

    void inst_jie(reg_name r, int offset)
    {
        if (reg(r) % 2 == 0) {
            inst_ptr_ += offset;
        }
        else {
            inst_ptr_++;
        }
    }

    void inst_jio(reg_name r, int offset)
    {
        if (reg(r) == 1) {
            inst_ptr_ += offset;
        }
        else {
            inst_ptr_++;
        }
    }
};

}  // namespace

aoc::solution_result day23(std::string_view input)
{
    const auto program{sv_lines(trim(input)) | r::to<std::vector>};
    cpu c1{program};
    c1.run();

    cpu c2{program};
    c2.reg(reg_name::a) = 1;
    c2.run();

    return {c1.reg(reg_name::b), c2.reg(reg_name::b)};
}

}  // namespace aoc::year2015
