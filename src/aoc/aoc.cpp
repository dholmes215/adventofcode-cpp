//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "aoc.hpp"

#include <range/v3/all.hpp>

#include <fmt/core.h>
#include <tl/expected.hpp>

#include <filesystem>
#include <fstream>

namespace aoc {
ifstream_expected open_file(const std::filesystem::path& datadir,
                            aoc::date date) noexcept
{
    const auto path_suffix{fmt::format("{:04}/{:02}.txt", date.year, date.day)};
    auto input_file{datadir / path_suffix};
    if (!std::filesystem::exists(input_file)) {
        return tl::unexpected{
            fmt::format("Input file does not exist: {}", input_file.string())};
    }

    // Load input
    std::ifstream file{input_file};

    if (!file.is_open()) {
        return tl::unexpected{
            fmt::format("Failed to open file: {}", input_file.string())};
    }

    return file;
}

std::string slurp(std::istream& stream)
{
    // return ranges::istream_view<char>(stream) | ranges::to<std::string>;
    const istream_range input{stream};
    return input | ranges::to<std::string>;
}

bool is_whitespace(char c)
{
    // Cast is necessary because `isspace` on a negative `char` is UB.
    return std::isspace(static_cast<unsigned char>(c));
}

}  // namespace aoc
