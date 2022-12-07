//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <map>
#include <string_view>
#include <variant>
#include <vector>

namespace aoc::year2022 {

namespace {

struct entry;
using file = std::size_t;
using dir = std::map<std::string_view, entry>;

struct entry {
    std::variant<dir, file> val;
};

std::size_t count_dir_size(const dir& d);

std::size_t entry_size(const entry& e)
{
    if (std::holds_alternative<file>(e.val)) {
        return std::get<file>(e.val);
    }
    return count_dir_size(std::get<dir>(e.val));
}

std::size_t count_dir_size(const dir& d)
{
    return r::accumulate(d | rv::values | rv::transform(entry_size), 0ULL);
}

void visit_all_dirs(const dir& d, const auto& visit)
{
    visit(d);
    for (const auto& [name, e] : d) {
        if (std::holds_alternative<dir>(e.val)) {
            visit_all_dirs(std::get<dir>(e.val), visit);
        }
    }
}

std::size_t sum_all_small_dirs(const dir& d)
{
    std::size_t out{0};
    auto visitor{[&](const dir& d2) {
        const auto size{count_dir_size(d2)};
        if (size <= 100000) {
            out += size;
        }
    }};
    visit_all_dirs(d, visitor);
    return out;
}

std::vector<std::size_t> collect_dir_sizes(const dir& d)
{
    std::vector<std::size_t> out;
    auto visitor{[&](const dir& d2) { out.push_back(count_dir_size(d2)); }};
    visit_all_dirs(d, visitor);
    return out;
}

}  // namespace

aoc::solution_result day07(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};

    dir root{};
    std::vector<dir*> working_dir{&root};

    if (lines.front() != "$ cd /") {
        throw input_error{"Expected first line to be \"$ cd /\""};
    }

    const auto end{lines.end()};
    auto line_iter{std::next(lines.begin())};
    while (line_iter != end) {
        auto line = *line_iter++;
        std::vector<std::string_view> words{sv_words(line) |
                                            r::to<std::vector>};

        if (words[0] != "$") {
            throw input_error{
                fmt::format("Expected prompt with command, not \"{}\"", line)};
        }

        auto command{words[1]};
        if (command == "ls") {
            while ((line_iter != end) && ((*line_iter)[0] != '$')) {
                // This is part of the ls output
                words = sv_words(*line_iter) | r::to<std::vector>;
                auto name{words[1]};
                if (words[0] == "dir") {
                    // Add this directory to the current directory
                    entry new_entry{dir{}};
                    auto& current_dir{*working_dir.back()};
                    current_dir.insert({name, new_entry});
                }
                else {
                    // Add this file to the working directory
                    std::size_t size{to_num<file>(words[0])};
                    working_dir.back()->insert({name, {size}});
                }
                line_iter++;
            }
        }
        else if (command == "cd") {
            auto new_dir_sv{words[2]};
            if (new_dir_sv == "..") {
                working_dir.pop_back();
            }
            else {
                auto& current_dir{*working_dir.back()};
                auto& entry_variant{current_dir.at(new_dir_sv).val};
                dir* new_dir{&std::get<dir>(entry_variant)};
                working_dir.push_back(new_dir);
            }
        }
        else {
            throw input_error{
                fmt::format("Unrecognized command: \"{}\"", line)};
        }
    }

    const auto root_size{count_dir_size(root)};
    const std::size_t unused_size{70000000ULL - root_size};
    const std::size_t update_size{30000000ULL};
    const std::size_t needed_space{update_size - unused_size};
    auto dir_sizes(collect_dir_sizes(root));
    r::sort(dir_sizes);
    const auto part2_size{
        *r::find_if(dir_sizes, [&](auto size) { return size > needed_space; })};

    return {sum_all_small_dirs(root), part2_size};
}

}  // namespace aoc::year2022
