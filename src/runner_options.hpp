//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef RUNNER_OPTIONS_HPP
#define RUNNER_OPTIONS_HPP

#include <filesystem>
#include <optional>

namespace aoc {

struct date_filter {
    int year{};
    std::optional<int> day;
};

struct runner_options {
    std::optional<std::filesystem::path> inputfile;
    std::optional<std::filesystem::path> datadir;
    std::optional<date_filter> dates;
};

runner_options process_args(int argc, char** argv);

}  // namespace aoc

#endif  // RUNNER_OPTIONS_HPP