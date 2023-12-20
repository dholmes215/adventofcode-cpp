//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/ranges.h>

#include <absl/container/flat_hash_map.h>

#include <algorithm>
#include <queue>
#include <set>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace aoc::year2023 {

namespace {

using int_t = std::int64_t;
using module_name_t = std::string_view;

struct module_t {
    module_name_t name{"!!!"};
    char type{'!'};
    std::vector<module_name_t> outputs;
    bool state{false};
};

#define MEMBER(member) [](const auto& o) { return o.member; }

struct name_index_map_t {
    std::vector<std::string_view> names_by_index;
    std::map<std::string_view, int> indexes_by_name;
};

module_t parse_module(std::string_view line)
{
    module_t out;
    auto words{sv_words(line)};
    auto iter{r::begin(words)};
    auto first{*iter++};
    if (first == "broadcaster") {
        out.name = "broadcaster";
        out.type = 'b';
    }
    else {
        out.name = first.substr(1);
        out.type = first[0];
    }
    iter++;  // ignore "->"
    while (iter != r::end(words)) {
        auto word{*iter++};
        if (word.ends_with(',')) {
            word = word.substr(0, word.size() - 1);
        }
        out.outputs.push_back(word);
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
    auto modules{sv_lines(trim(input)) | rv::transform(parse_module) |
                 r::to<std::vector>};

    std::set<module_name_t> module_names{modules | rv::transform(MEMBER(name)) |
                                         r::to<std::set>};

    absl::flat_hash_map<module_name_t, module_t> modules_by_name;
    for (const module_t& module : modules) {
        modules_by_name[module.name] = module;
    }
    for (const module_t& module : modules) {
        for (module_name_t output : module.outputs) {
            if (!modules_by_name.contains(output)) {
                // An output that isn't listed as a module
                modules_by_name[output] = module_t{output, 'o', {}, false};
            }
        }
    }

    absl::flat_hash_map<module_name_t, std::map<module_name_t, bool>>
        input_memories;
    for (const module_t& module : modules) {
        auto input_name{module.name};
        for (module_name_t output_name : module.outputs) {
            const module_t& output{modules_by_name[output_name]};
            if (output.type == '&') {
                input_memories[output_name][input_name] = false;
            }
        }
    }

    std::queue<pulse_t> pulse_queue;

    int_t low_pulses_sent{0};
    int_t high_pulses_sent{0};
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
            rx_sent |= pulse.dest == "rx";
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
                    bool output_pulse{
                        !r::all_of(input_memories[module.name] | rv::values,
                                   std::identity{})};
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
    for (int_t i{0}; i < 1000 || !rx_sent; i++) {
        send_pulse({"button", "broadcaster", false});
        process_queue();
        if (part2 == 0 && rx_sent) {
            part2 = i;
        }
        if ((i % 1000000) == 0) {
            fmt::print("{}\n", i);
        }
    }

    const int_t part1{low_pulses_sent * high_pulses_sent};

    return {part1, part2};
}

}  // namespace aoc::year2023
