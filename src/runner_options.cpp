//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "runner_options.hpp"

#include <aoc_range.hpp>

#include <fmt/core.h>
#include <cxxopts.hpp>

namespace {

// Auto-detect the AoC input data directory (for when none was provided).
std::optional<std::filesystem::path> find_data_dir()
{
    auto path = std::filesystem::current_path();
    while (!std::filesystem::exists(path / "data")) {
        auto last_path_size = path.string().size();
        path = path.parent_path();
        if (path.string().size() >= last_path_size) {
            return {};
        }
    }
    return std::filesystem::canonical(path / "data");
}

}  // namespace

namespace aoc {

runner_options process_args(int argc, char** argv)
{
    runner_options out;

    cxxopts::Options options("runner", "Advent of Code solution runner");
    // clang-format off
    // TODO: figure out how to do `./runner --whatever abc 2021 01` without --year and --day
    options.add_options()
        ("year", "Run only a single year's solutions", cxxopts::value<int>())
        ("day", "Run only a single day's solution (requires --year)", cxxopts::value<int>())
        ("datadir", "Input data directory (excludes --inputfile)", cxxopts::value<std::string>())
        ("inputfile", "Input file (requires --day, excludes --datadir)", cxxopts::value<std::string>())
        ("repeat", "Repeat each solution this many times (default: 1)", cxxopts::value<int>())
        ("seconds", "Repeat each solution at most this long (default: 1)", cxxopts::value<int>());
    // clang-format on
    auto parsed_options{options.parse(argc, argv)};
    if (parsed_options.count("datadir") > 0) {
        const auto datadir{parsed_options["datadir"].as<std::string>()};
        const std::filesystem::path datadir_path{datadir};
        if (!std::filesystem::exists(datadir_path)) {
            fmt::print(stderr, "Data directory '{}' does not exist\n", datadir);
            std::abort();
        }
        out.datadir = std::filesystem::canonical(datadir_path);
    }
    else {
        out.datadir = find_data_dir();
    }

    if (parsed_options.count("year") > 0) {
        out.dates.emplace();
        out.dates->year = parsed_options["year"].as<int>();
    }

    if (parsed_options.count("day") > 0) {
        if (!out.dates) {
            fmt::print(stderr, "--day requires --year\n");
            std::abort();
        }
        out.dates->day = parsed_options["day"].as<int>();
    }

    if (parsed_options.count("inputfile") > 0) {
        if (!out.dates) {
            fmt::print(stderr, "--inputfile requires --day\n");
            std::abort();
        }
        if (parsed_options.count("datadir") > 0) {
            fmt::print(stderr,
                       "--inputfile and --datadir are mutually exclusive\n");
            std::abort();
        }
        out.inputfile = std::filesystem::path{
            parsed_options["inputfile"].as<std::string>()};
        if (!std::filesystem::exists(*out.inputfile)) {
            fmt::print(stderr, "Input file '{}' does not exist\n",
                       out.inputfile->string());
            std::abort();
        }
    }

    if (!out.inputfile && !out.datadir) {
        fmt::print(stderr,
                   "Neither --inputfile or --datadir was provided and no data "
                   "directory was found by search.  Create a data directory, "
                   "or provide either --inputfile or --datadir.\n");
        std::abort();
    }

    if (parsed_options.count("repeat") > 0) {
        out.repeat = parsed_options["repeat"].as<int>();
    }

    if (parsed_options.count("seconds") > 0) {
        out.seconds = parsed_options["seconds"].as<int>();
    }

    return out;
}

}  // namespace aoc
