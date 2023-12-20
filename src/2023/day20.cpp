//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <algorithm>
#include <bitset>
#include <queue>
#include <string_view>
#include <vector>

namespace aoc::year2023 {

namespace {

using int_t = std::int64_t;
using module_name_t = std::int8_t;
using module_set_t = std::bitset<64>;

struct module_t {
    module_name_t name{-1};
    char type{'!'};
    std::vector<module_name_t> outputs;
    module_set_t input_mask;
    bool state{false};
};

#define MEMBER(member) [](const auto& o) { return o.member; }

constexpr const std::string_view part2_output_name{"rx"};

std::string_view trim_non_letters(std::string_view input)
{
    while (!input.empty() && !is_letter(input.front())) {
        input = input.substr(1);
    }
    while (!input.empty() && !is_letter(input.back())) {
        input = input.substr(0, input.size() - 1);
    }
    return input;
}

std::vector<std::string_view> parse_and_sort_names(std::string_view input)
{
    std::vector<std::string_view> names;
    names.push_back("button");

    auto words{sv_words(input)};
    for (std::string_view word : words) {
        word = trim_non_letters(word);
        if (!word.empty()) {
            names.push_back(word);
        }
    }

    r::sort(names);
    names.erase(r::unique(names), r::end(names));

    return names;
}

std::int8_t get_index_for_name(
    const std::vector<std::string_view>& sorted_names,
    std::string_view name)
{
    if (sorted_names.size() > 64) {
        throw input_error("too many names");
    }
    const auto lower{r::lower_bound(sorted_names, name)};
    if (lower == r::end(sorted_names) || *lower != name) {
        throw input_error(fmt::format("could not find name: {}", name));
    }
    return static_cast<int8_t>(r::distance(r::begin(sorted_names), lower));
}

module_t parse_module(std::string_view line,
                      const std::vector<std::string_view>& sorted_names)
{
    const auto to_idx{[&](std::string_view name) {
        return get_index_for_name(sorted_names, name);
    }};
    module_t out;
    auto words{sv_words(line)};
    auto iter{r::begin(words)};
    auto first{*iter++};
    if (first == "broadcaster") {
        out.name = to_idx("broadcaster");
        out.type = 'b';
    }
    else {
        out.name = to_idx(first.substr(1));
        out.type = first[0];
    }
    iter++;  // ignore "->"
    while (iter != r::end(words)) {
        auto word{*iter++};
        if (word.ends_with(',')) {
            word = word.substr(0, word.size() - 1);
        }
        out.outputs.push_back(to_idx(word));
    }
    return out;
}

struct pulse_t {
    module_name_t source;
    module_name_t dest;
    bool pulse;
};

}  // namespace

aoc::solution_result day20(std::string_view input)
{
    const auto sorted_names{parse_and_sort_names(input)};
    const auto to_idx{[&](std::string_view name) {
        return get_index_for_name(sorted_names, name);
    }};
    auto modules{sv_lines(trim(input)) |
                 rv::transform([&](std::string_view line) {
                     return parse_module(line, sorted_names);
                 }) |
                 r::to<std::vector>};

    std::vector<module_t> modules_by_name(sorted_names.size());
    for (const module_t& module : modules) {
        modules_by_name[module.name] = module;
    }
    for (const module_t& module : modules) {
        for (module_name_t output : module.outputs) {
            if (modules_by_name[output].name == -1) {
                // An output that isn't listed as a module
                modules_by_name[output] = module_t{output, 'o', {}, false};
            }
        }
    }

    // Visualize modules
    fmt::print("digraph \"modules\" {}\n", '{');
    for (const module_t& module : modules_by_name) {
        for (module_name_t output : module.outputs) {
            fmt::print("    {} -> {}\n", sorted_names[module.name],
                       sorted_names[output]);
        }
    }
    fmt::print("{}\n", '}');

    std::vector<module_set_t> input_memories(sorted_names.size());
    for (const module_t& module : modules) {
        auto input_name{module.name};
        for (module_name_t output_name : module.outputs) {
            module_t& output{modules_by_name[output_name]};
            output.input_mask[input_name] = true;
        }
    }

    std::queue<pulse_t> pulse_queue;

    int_t low_pulses_sent{0};
    int_t high_pulses_sent{0};
    std::vector<module_name_t> grandparents{to_idx("dp"), to_idx("dh"),
                                            to_idx("qd"), to_idx("bb")};
    std::map<module_name_t, bool> grandparents_current;
    module_name_t rx_index{to_idx("rx")};
    bool rx_sent{false};
    const auto send_pulse{[&](pulse_t pulse) {
        // fmt::print("{} -{}-> {}\n", pulse.source, pulse.pulse ? "high" :
        // "low", pulse.dest);
        pulse_queue.push(pulse);
        if (pulse.pulse) {
            high_pulses_sent++;
        }
        else {
            low_pulses_sent++;
            rx_sent |= pulse.dest == rx_index;

            for (module_name_t grandparent : grandparents) {
                if (pulse.dest == grandparent) {
                    grandparents_current[grandparent] = true;
                }
            }
        }
    }};

    const auto process_queue{[&]() {
        while (!pulse_queue.empty()) {
            pulse_t pulse{pulse_queue.front()};
            pulse_queue.pop();
            module_t& module{modules_by_name[pulse.dest]};
            // fmt::print("  processing {} -{}-> {}\n", pulse.source,
            //            pulse.pulse ? "high" : "low", pulse.dest);
            switch (module.type) {
                case 'b':
                    for (const module_name_t& output : module.outputs) {
                        send_pulse({module.name, output, pulse.pulse});
                    }
                    break;
                case '%':
                    if (pulse.pulse) {
                        // High; do nothing
                    }
                    else {
                        module.state = !module.state;
                        for (const module_name_t& output : module.outputs) {
                            send_pulse({module.name, output, module.state});
                        }
                    }
                    break;
                case '&': {
                    input_memories[module.name][pulse.source] = pulse.pulse;
                    bool output_pulse{input_memories[module.name] !=
                                      module.input_mask};
                    for (const module_name_t& output : module.outputs) {
                        send_pulse({module.name, output, output_pulse});
                    }
                    break;
                }
                case 'o':
                    break;
                default:
                    throw input_error(fmt::format("unsupported module type: {}",
                                                  module.type));
            }
        }
    }};

    int_t part2{0};

    module_name_t button_idx{to_idx("button")};
    module_name_t broadcaster_idx{to_idx("broadcaster")};
    for (int_t i{1}; i < 1001 || !rx_sent; i++) {
        send_pulse({button_idx, broadcaster_idx, false});
        process_queue();
        if (part2 == 0 && rx_sent) {
            part2 = i;
        }
        if ((i % 1000000) == 0) {
            fmt::print("{}\n", i);
        }

        for (module_name_t grandparent : grandparents) {
            if (grandparents_current[grandparent]) {
                fmt::print("{}: {}\n", i, sorted_names[grandparent]);
            }
        }
        grandparents_current.clear();
    }

    const int_t part1{low_pulses_sent * high_pulses_sent};

    return {part1, part2};
}

}  // namespace aoc::year2023
