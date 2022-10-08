//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <ctre.hpp>

#include <nlohmann/json.hpp>

#include <cstdint>
#include <string_view>

namespace aoc::year2015 {

namespace {

using json_number_t = std::uint64_t;

json_number_t part1(std::string_view input)
{
    // XXX The ctre::range docs say this API might change.  If the build fails
    // here, go see what it changed to.
    constexpr auto& pattern{
        R"((\-?)(0|(?:[1-9]\d*))(\.\d+)?([eE][\+\-]?\d+)?)"};
    const auto range{ctre::range<pattern>(input)};
    // XXX All the input numbers are ints, but I wrote the above regex to
    // support any JSON number before I realized that.

    using match_type = decltype(*range.begin());
    const auto match_to_int{
        [](match_type match) { return to_int(match.get<0>().to_view()); }};
    auto int_range{range | rv::transform(match_to_int)};
    return r::accumulate(int_range, json_number_t{});
}

// Part One could be solved simply by extracting all the ints from the text file
// and ignoring the structure, but Part Two sadistically requires some amount of
// actual parsing.  Rather than parse it ourselves, we'll use a library.

// Exception thrown by solutions on unexpected JSON contents.
class json_input_error : public std::runtime_error {
   public:
    json_input_error(std::string msg) : runtime_error{std::move(msg)} {}
};

bool is_red(const nlohmann::json& value)
{
    if (!value.is_string()) {
        return false;
    }
    return value.get<std::string>() == "red";
}

json_number_t sum_tree(const nlohmann::json& json);

json_number_t sum_object(const nlohmann::json& object)
{
    const auto items{object.items()};
    const auto values{items |
                      rv::transform([](auto item) { return item.value(); })};
    if (r::any_of(values, is_red)) {
        // If any member is "red", ignore this whole object
        return json_number_t{};
    }
    return r::accumulate(values | rv::transform(sum_tree), json_number_t{});
}

json_number_t sum_tree(const nlohmann::json& json)
{
    if (json.is_array()) {
        return r::accumulate(json | rv::transform(sum_tree), json_number_t{});
    }
    else if (json.is_number()) {
        return json.get<json_number_t>();
    }
    else if (json.is_object()) {
        return sum_object(json);
    }
    else if (json.is_string()) {
        return json_number_t{};  // 0
    }

    throw json_input_error{"JSON included some type we don't handle"};
}

json_number_t part2(std::string_view input)
{
    const auto parsed_json = nlohmann::json::parse(input);
    if (!parsed_json.is_object()) {
        throw json_input_error{"JSON input is unexpectedly not an object"};
    }

    return sum_tree(parsed_json);
}

}  // namespace

aoc::solution_result day12(std::string_view input)
{
    return {part1(input), part2(input)};
}

}  // namespace aoc::year2015
