//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>
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
#include <string_view>

namespace aoc {

void run_solution(aoc::date date,
                  const solution& sol,
                  std::string_view input,
                  const runner_options& options)
{
    using std::chrono::duration_cast;
    using std::chrono::microseconds;
    using std::chrono::seconds;
    using Clock = std::chrono::high_resolution_clock;
    using FpMicroseconds = std::chrono::duration<float, microseconds::period>;

    const auto start{Clock::now()};

    const auto& result{sol.func(input)};
    int i{1};
    for (; i < options.repeat; i++) {
        const auto end_iteration{Clock::now()};
        const auto elapsed_seconds{
            duration_cast<seconds>(end_iteration - start)};
        if (elapsed_seconds.count() >= options.seconds) {
            break;
        }

        // Solve problem
        const auto& new_result{sol.func(input)};
        if (new_result != result) {
            fmt::print(
                "{1:20} {2:10} {0:red}{3:>20} {4:>20} "
                "{5:}{0:reset}\n",
                dh::color{}, date, sol.label, new_result.part_a,
                new_result.part_b, "inconsistent result on repeated iteration");
        }
    }
    const auto end{Clock::now()};
    const auto elapsed{duration_cast<FpMicroseconds>(end - start)};
    const auto avg_elapsed{elapsed / i};
    fmt::print("{:20} {:10} {:>20} {:>20} {:>15} {:>10}\n", date, sol.label,
               result.part_a, result.part_b, avg_elapsed, i);
}

}  // namespace aoc

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
    aoc::date begin_date{aoc::solutions().begin()->first};
    aoc::date end_date{std::prev(aoc::solutions().end())->first};
    if (options.dates) {
        begin_date.year = options.dates->year;
        begin_date.day = options.dates->day ? *options.dates->day : 1;
        end_date.year = options.dates->year;
        end_date.day = options.dates->day ? *options.dates->day : 25;
    }
    fmt::print(
        "Running solutions from {0:red}{1}{0:reset} to {0:green}{2}{0:reset} "
        "{3} times\n",
        dh::color{}, begin_date, end_date, options.repeat);
    fmt::print("{:20} {:10} {:>20} {:>20} {:>15} {:>10}\n", "Day", "Solution",
               "Part A", "Part B", "Duration", "Iterations");
    const auto solutions_range{
        aoc::submap(aoc::solutions(), begin_date, end_date)};
    for (const auto& [date, solution_vec] : solutions_range) {
        // Load input
        auto maybe_input{options.inputfile
                             ? aoc::open_file(*options.inputfile)
                             : aoc::open_file(*options.datadir, date)};
        if (maybe_input) {
            const auto input{aoc::slurp(*maybe_input)};
            for (const auto& solution : solution_vec) {
                try {
                    run_solution(date, solution, input, options);
                }
                catch (std::runtime_error& e) {
                    fmt::print("{:20} {:10} Exception thrown: {}\n", date,
                               solution.label, e.what());
                }
            }
        }
        else {
            fmt::print("{:20} {}\n", date, maybe_input.error());
        }
    }
}
