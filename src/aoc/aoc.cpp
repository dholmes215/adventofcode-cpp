//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "aoc.hpp"
#include "aoc_range.hpp"

#include <fmt/core.h>
#include <tl/expected.hpp>

#include <filesystem>
#include <fstream>
#include <functional>
#include <string_view>

namespace aoc {
ifstream_expected open_file(const std::filesystem::path& datadir,
                            aoc::date date) noexcept
{
    const auto path_suffix{fmt::format("{:04}/{:02}.txt", date.year, date.day)};
    return open_file(datadir / path_suffix);
}

ifstream_expected open_file(const std::filesystem::path& inputfile) noexcept
{
    if (!std::filesystem::exists(inputfile)) {
        return tl::unexpected{
            fmt::format("Input file does not exist: {}", inputfile.string())};
    }

    // Load input
    std::ifstream file{inputfile};

    if (!file.is_open()) {
        return tl::unexpected{
            fmt::format("Failed to open file: {}", inputfile.string())};
    }

    return file;
}

namespace {
// Helper to allow iterating the `char`s in an `istream` with a ranged for loop.
struct istream_range {
   public:
    using Iterator = std::istreambuf_iterator<char>;
    istream_range(std::istream& stream) : begin_iter(stream) {}
    Iterator begin() const noexcept { return begin_iter; }
    Iterator end() const noexcept { return {}; }

   private:
    Iterator begin_iter{};
};
}  // namespace

std::string slurp(std::istream& stream)
{
    // return ranges::istream_view<char>(stream) | ranges::to<std::string>;
    const istream_range input{stream};
    return input | r::to<std::string>;
}

bool is_whitespace(char c)
{
    // Cast is necessary because `isspace` on a negative `char` is UB.
    return std::isspace(static_cast<unsigned char>(c));
}

// Strip leading and trailing whitespace from a string, including newlines.
std::string_view trim(std::string_view s) noexcept
{
    const auto b = r::find_if_not(s, is_whitespace);
    if (b == s.end()) {
        return {};
    }
    const auto e = r::find_if_not(s | rv::reverse, is_whitespace);
    return {&*b, (&*e) + 1};
}

}  // namespace aoc
