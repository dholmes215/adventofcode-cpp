//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_solutions.hpp>
#include <runner_options.hpp>

#include <term.hpp>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <cxxopts.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

int main(int argc, char** argv)
{
    const auto options{aoc::process_args(argc, argv)};

    if (options.datadir) {
        fmt::print("Using data directory: {}\n", options.datadir->string());
    }
    else {
        fmt::print("Using input file: {}\n", options.inputfile->string());
    }

    // Select range of dates to run.
    aoc::solution_id begin_date{aoc::solutions().begin()->first};
    aoc::solution_id end_date{std::prev(aoc::solutions().end())->first};
    if (options.dates) {
        begin_date.year = options.dates->year;
        begin_date.day = options.dates->day ? *options.dates->day : 1;
        end_date.year = options.dates->year;
        end_date.day = options.dates->day ? *options.dates->day : 25;
    }
    fmt::print(
        "Running solutions from {0:red}{1}{0:reset} to {0:green}{2}{0:reset}\n",
        dh::color{}, begin_date, end_date);
    fmt::print("{:20} {:>20} {:>20} {:>10}\n", "Day", "Part A", "Part B",
               "Duration");
    const auto solutions_range{
        aoc::submap(aoc::solutions(), begin_date, end_date)};
    for (const auto& [id, solution] : solutions_range) {
        // Load input
        auto maybe_input{aoc::open_file(*options.datadir, id)};
        if (maybe_input) {
            // Solve problem
            using std::chrono::duration_cast;
            using std::chrono::microseconds;
            using Clock = std::chrono::high_resolution_clock;
            using FpMicroseconds =
                std::chrono::duration<float, microseconds::period>;

            const auto start{Clock::now()};
            const auto& result{solution(*maybe_input)};
            const auto end{Clock::now()};
            const auto elapsed{duration_cast<FpMicroseconds>(end - start)};
            fmt::print("{:20} {:>20} {:>20} {:>10}\n", id, result.part_a,
                       result.part_b, elapsed);
        }
        else {
            fmt::print("{:20} {}\n", id, maybe_input.error());
        }
    }
}
