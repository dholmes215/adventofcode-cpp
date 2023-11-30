//
// Copyright (c) 2020-2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <cmrc/cmrc.hpp>
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <fmt/std.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

CMRC_DECLARE(aoc);

namespace aoc {

struct generate_options {
    std::string year;
    std::string copyright_years;
    std::filesystem::path src_path;
    bool overwrite{false};
};

generate_options parse_options(int argc, char** argv)
{
    generate_options out;
    cxxopts::Options options("generate_year",
                             "Advent of Code skeleton generator utility");
    // clang-format off
    options.add_options()
        ("year", "Year to generate", cxxopts::value<std::string>())
        ("copyright_years", "Years to appear in copyright notice (eg. \"2020-2022\")", cxxopts::value<std::string>())
        ("src", "Path to parent directory of year to generate", cxxopts::value<std::string>())
        ("overwrite", "Overwrite existing files", cxxopts::value<bool>())
        ("h,help", "Print usage");
    // clang-format on
    auto parsed_options{options.parse(argc, argv)};

    if (parsed_options.count("help")) {
        fmt::print(stderr, "{}\n", options.help());
        std::exit(0);
    }

    if (parsed_options.count("year") > 0) {
        out.year = parsed_options["year"].as<std::string>();
    }
    else {
        fmt::print(stderr, "ERROR: Missing argument: --year [year]\n\n{}\n",
                   options.help());
        std::exit(-1);
    }

    if (parsed_options.count("copyright_years") > 0) {
        out.copyright_years =
            parsed_options["copyright_years"].as<std::string>();
    }
    else {
        fmt::print(stderr,
                   "ERROR: Missing argument: --copyright_years [years]\n\n{}\n",
                   options.help());
        std::exit(-1);
    }

    if (parsed_options.count("src") > 0) {
        out.src_path = parsed_options["src"].as<std::string>();
        if (!std::filesystem::exists(out.src_path)) {
            fmt::print(stderr, "Source directory '{}' does not exist\n",
                       out.src_path);
            std::exit(-1);
        }
        out.src_path = std::filesystem::canonical(out.src_path);
    }
    else {
        fmt::print(stderr,
                   "ERROR: Missing argument: --src /path/to/src\n\n{}\n",
                   options.help());
        std::exit(-1);
    }

    if (parsed_options.count("overwrite") > 0) {
        out.overwrite = parsed_options["overwrite"].as<bool>();
    }

    return out;
}

void generate_file(const char* template_filename,
                   std::filesystem::path out_filepath,
                   const auto& replacements)
{
    auto fs{cmrc::aoc::get_filesystem()};
    auto in_file{fs.open(template_filename)};

    std::string contents;
    contents.resize(in_file.size());
    aoc::r::copy(in_file, contents.begin());

    std::ofstream out_file{out_filepath, std::ios::binary | std::ios::out};
    auto iter{contents.cbegin()};
    const auto end{contents.cend()};

    while (iter < end) {
        const std::string_view rest{iter, end};
        bool found_replacement{false};
        for (auto [substr, replacement] : replacements) {
            if (rest.starts_with(substr)) {
                out_file.write(replacement.data(), static_cast<std::streamsize>(
                                                       replacement.size()));
                iter +=
                    static_cast<decltype(iter)::difference_type>(substr.size());
                found_replacement = true;
                break;
            }
        }
        if (!found_replacement) {
            out_file.put(*iter);
            iter++;
        }
    }
    fmt::print("Wrote {}.\n", out_filepath);
}

}  // namespace aoc

int main(int argc, char** argv)
{
    const auto options{aoc::parse_options(argc, argv)};

    const auto year_path{options.src_path / options.year};
    std::filesystem::create_directories(year_path);

    const auto check_exists{[&](const std::filesystem::path& path) {
        if (std::filesystem::exists(path) && !options.overwrite) {
            fmt::print(
                stderr,
                "File already exists and --overwrite was not enabled: {}\n",
                path);
            std::exit(-1);
        }
    }};

    const auto cmakelists_path{year_path / "CMakeLists.txt"};
    check_exists(cmakelists_path);
    const auto yearcpp_path{year_path /
                            fmt::format("year{}.hpp", options.year)};
    check_exists(yearcpp_path);

    for (int day{1}; day <= 25; day++) {
        const auto day_path{year_path / fmt::format("day{:02}.cpp", day)};
        check_exists(day_path);
    }

    std::vector<std::pair<std::string_view, std::string_view>> replacements{
        {"%YEAR%", options.year},
        {"%COPYRIGHT_YEARS%", options.copyright_years}};
    aoc::generate_file("template_CMakeLists.txt", cmakelists_path,
                       replacements);
    aoc::generate_file("template_year.hpp.txt", yearcpp_path, replacements);
    for (int day{1}; day <= 25; day++) {
        std::string day_string{fmt::format("{:02}", day)};
        const auto day_path{year_path / fmt::format("day{:02}.cpp", day)};
        replacements.push_back({"%DAY%", day_string});
        aoc::generate_file("template_day.cpp.txt", day_path, replacements);
        replacements.pop_back();
    }
}
