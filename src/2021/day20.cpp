//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <bitset>
#include <string>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

namespace {

using enhance_alg_t = std::bitset<512>;
using grid_t = dynamic_grid<bool>;

enhance_alg_t parse_enhance_alg(std::string_view s)
{
    std::string reversed{s | rv::reverse | r::to<std::string>};
    return std::bitset<512>(reversed.data(), reversed.size(), '.', '#');
}

struct parse_result {
    enhance_alg_t enhance_alg;
    grid_t grid;
};

parse_result parse(std::string_view input)
{
    auto lines{sv_lines(input)};
    auto rest{lines | rv::drop(2)};
    const int grid_width{static_cast<int>(rest.front().size())};
    const int grid_height{static_cast<int>(r::distance(rest))};

    parse_result out{parse_enhance_alg(lines.front()),
                     grid_t{grid_width, grid_height}};
    auto bools{rest | rv::join |
               rv::transform([](char c) { return c == '#'; })};
    r::copy(bools, out.grid.data().begin());
    return out;
}

grid_t widen(grid_t& grid, int padding, bool new_value)
{
    grid_t widened{grid.width() + padding * 2, grid.height() + padding * 2};
    if (new_value) {
        r::fill(widened.data(), true);
    }
    r::copy(grid.data(),
            widened.subgrid({{padding, padding}, {grid.width(), grid.height()}})
                .data()
                .begin());
    return widened;
}

grid_t enhance(grid_t& grid, const enhance_alg_t& enhance_alg)
{
    grid_t widened{widen(grid, 2, grid[{0, 0}])};

    grid_t out{grid.width() + 2, grid.height() + 2};
    for (const auto p : out.area().all_points()) {
        auto nine_pixel_subgrid{widened.subgrid({p, {3, 3}})};

        auto index{static_cast<std::size_t>(
            bool_range_to_int(nine_pixel_subgrid.data()))};
        out[p] = enhance_alg[index];
    }

    return out;
}

}  // namespace

aoc::solution_result day20(std::string_view input)
{
    auto [enhance_alg, grid]{parse(input)};

    std::vector<grid_t> enhancements;
    enhancements.emplace_back(widen(grid, 10, false));
    for (std::size_t i = 1; i <= 50; i++) {
        enhancements.emplace_back(enhance(enhancements[i - 1], enhance_alg));
    }

    return {r::count(enhancements[2].data(), true),
            r::count(enhancements[50].data(), true)};
}

}  // namespace aoc::year2021
