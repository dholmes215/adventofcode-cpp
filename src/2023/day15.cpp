//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <algorithm>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

using int_t = std::int32_t;
using hash_t = std::uint8_t;

auto split_input(std::string_view input)
{
    return sv_split_range(trim(input), ',');
}

constexpr hash_t hash(std::string_view s)
{
    int_t hash{0};
    for (char c : s) {
        hash += c;
        hash *= 17;
        hash %= 256;
    }
    return static_cast<hash_t>(hash);
}

static_assert(hash("HASH") == 52);

struct instruction {
    std::string_view instruction;
    std::string_view label;
    char operation;
    int_t focal_length;
};

instruction parse_instruction(std::string_view i)
{
    auto op_idx{i.find_first_of("=-")};
    return {i, i.substr(0, op_idx), i[op_idx],
            (i[op_idx] == '=') ? to_num<int_t>(i.substr(op_idx + 1)) : 0};
}

class hashmap {
   public:
    using value_type = std::pair<std::string_view, int_t>;
    using key_type = std::string_view;
    void insert(const value_type& p)
    {
        auto& [key, value]{p};
        hash_t h{hash(key)};
        auto& vec{data_[h]};
        auto found{r::find(vec, key, pair_key_)};
        if (found == vec.end()) {
            vec.push_back(p);
        }
        else {
            *found = p;
        }
    }

    void remove(const key_type& key)
    {
        hash_t h{hash(key)};
        auto& vec{data_[h]};
        std::erase_if(vec, [&](const value_type& p) { return p.first == key; });
    }

    friend int_t focusing_power(const hashmap& h);

   private:
    std::array<std::vector<value_type>, 256> data_{};
    static key_type pair_key_(const value_type& p) { return p.first; }
};

int_t focusing_power(const hashmap& h)
{
    int_t out{0};
    for (const auto& [box, vec] : rv::zip(rv::iota(1), h.data_)) {
        for (const auto& [slot, pair] : rv::zip(rv::iota(1), vec)) {
            const auto& [label, focal_length]{pair};
            out += box * slot * focal_length;
        }
    }
    return out;
}

}  // namespace

aoc::solution_result day15(std::string_view input)
{
    auto part1{
        r::accumulate(split_input(input) | rv::transform(hash), int_t{0})};

    hashmap h;
    for (instruction i :
         split_input(input) | rv::transform(parse_instruction)) {
        if (i.operation == '=') {
            h.insert({i.label, i.focal_length});
        }
        if (i.operation == '-') {
            h.remove(i.label);
        }
    }

    auto part2{focusing_power(h)};

    return {part1, part2};
}

}  // namespace aoc::year2023
