//
// Copyright (c) 2020-2023 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>

#include <term.hpp>

#include <fmt/ranges.h>

#include <algorithm>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace aoc::year2023 {

namespace {

using int_t = std::int64_t;
using grid_t = dynamic_grid<char>;
using pos_t = vec2<int>;

constexpr const int part2_expansion_factor{13};

// constexpr const int part2_step_goal{26501365};
constexpr const int part2_step_goal{5000};

constexpr const pos_t up{0, -1};
constexpr const pos_t down{0, 1};
constexpr const pos_t left{-1, 0};
constexpr const pos_t right{1, 0};

constexpr const std::array<pos_t, 4> cardinal_directions{
    {up, down, left, right}};

grid_t parse_grid(std::string_view input)
{
    const auto lines{sv_lines(trim(input)) | r::to<std::vector>};
    grid_t grid{static_cast<int>(lines[0].size()),
                static_cast<int>(lines.size())};
    const auto tiles{lines | rv::join |
                     rv::transform([](char c) { return c; })};
    r::copy(tiles, grid.data().data());
    return grid;
}

pos_t find_start(grid_t& grid)
{
    for (pos_t p : grid.area().all_points()) {
        if (grid[p] == 'S') {
            grid[p] = '.';
            return p;
        }
    }
    throw input_error("failed to find start");
}

void print_grid(const grid_t& grid)
{
    for (int row{0}; row < grid.height(); row++) {
        if (row % (grid.height() / part2_expansion_factor) == 0) {
            fmt::print("\n");
        }
        for (int col{0}; col < grid.width(); col++) {
            if (col % (grid.width() / part2_expansion_factor) == 0) {
                fmt::print(" ");
            }
            fmt::print("{}", grid[{col, row}]);
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

grid_t expand_grid(const grid_t& grid)
{
    grid_t out{grid.width() * part2_expansion_factor,
               grid.height() * part2_expansion_factor};
    for (int y{0}; y < part2_expansion_factor; y++) {
        for (int x{0}; x < part2_expansion_factor; x++) {
            auto subgrid{out.subgrid({{x * grid.width(), y * grid.height()},
                                      {grid.width(), grid.height()}})};
            r::copy(grid.data(), subgrid.data().begin());
        }
    }
    return out;
}

template <typename GridType>
struct grid_hash {
    std::size_t operator()(const GridType& grid) const noexcept
    {
        std::size_t h{std::hash<int>{}(grid.width())};
        h = h * 37 + std::hash<int>{}(grid.height());
        for (char c : grid.data()) {
            h = h * 37 + std::hash<char>{}(c);
        }
        return h;
    }
};

template <typename GridType>
struct grid_equal {
    bool operator()(const GridType& a, const GridType& b) const noexcept
    {
        return r::equal(a.data(), b.data());
    }
};

template <typename A, typename B>
const std::pair<B, A> transpose_pair(const std::pair<A, B>& p)
{
    return {p.second, p.first};
}

}  // namespace

aoc::solution_result day21(std::string_view input)
{
    grid_t start_grid{parse_grid(input)};
    pos_t start{find_start(start_grid)};

    grid_t current_grid{start_grid};
    grid_t next_grid{start_grid};
    current_grid[start] = 'O';
    // print_grid(current_grid);
    for (int i{0}; i < 64; i++) {
        for (pos_t p : current_grid.area().all_points()) {
            if (current_grid[p] == 'O') {
                auto neighbors{cardinal_directions |
                               rv::transform([&](pos_t p2) { return p2 + p; }) |
                               rv::filter([&](pos_t p2) {
                                   return current_grid.area().contains(p2) &&
                                          current_grid[p2] == '.';
                               })};

                for (pos_t neighbor : neighbors) {
                    next_grid[neighbor] = 'O';
                }
            }
        }

        current_grid = next_grid;
        next_grid = start_grid;
        // fmt::print("{}: {}\n", i+1, r::count(current_grid.data(), 'O'));
        // print_grid(current_grid);
    }

    const int_t part1{r::count(current_grid.data(), 'O')};

    grid_t expanded_grid{expand_grid(start_grid)};
    pos_t expanded_start{
        start + (start_grid.area().dimensions * (part2_expansion_factor / 2))};

    std::vector<grid_t> grids_by_step{expanded_grid};
    grids_by_step[0][expanded_start] = 'O';
    // print_grid(grids_by_step[0]);

    using SubgridType = decltype(expanded_grid.subgrid({1, 1}));
    std::unordered_map<std::size_t, int> discovered_subgrid_indexes;
    std::vector<grid_t> discovered_subgrids_by_index;
    int current_discovery{0};
    const auto discover_subgrid{[&](const grid_t& grid, int subx, int suby) {
        auto subgrid{grid.subgrid(
            {{subx * start_grid.width(), suby * start_grid.height()},
             {start_grid.width(), start_grid.height()}})};

        auto hash{grid_hash<decltype(subgrid)>{}(subgrid)};
        if (!discovered_subgrid_indexes.contains(hash)) {
            discovered_subgrid_indexes[hash] = current_discovery;
            discovered_subgrids_by_index.emplace_back(subgrid.width(),
                                                      subgrid.height());
            auto& back{discovered_subgrids_by_index.back()};
            r::copy(subgrid.data(), back.data().begin());

            current_discovery++;
        }
    }};

    for (int y{0}; y < part2_expansion_factor; y++) {
        for (int x{0}; x < part2_expansion_factor; x++) {
            discover_subgrid(grids_by_step[0], x, y);
        }
    }

    std::size_t previous_subgrid_count{1};
    int last_subgrid_count_change{0};
    for (int i{0}; i - last_subgrid_count_change < 100; i++) {
        grids_by_step.push_back(expanded_grid);
        auto& current{(grids_by_step | rv::reverse)[1]};
        auto& next{(grids_by_step | rv::reverse)[0]};
        for (pos_t p : current.area().all_points()) {
            if (current[p] == 'O') {
                auto neighbors{cardinal_directions |
                               rv::transform([&](pos_t p2) { return p2 + p; }) |
                               rv::filter([&](pos_t p2) {
                                   return current.area().contains(p2) &&
                                          current[p2] == '.';
                               })};

                for (pos_t neighbor : neighbors) {
                    next[neighbor] = 'O';
                }
            }
        }

        for (int y{0}; y < part2_expansion_factor; y++) {
            for (int x{0}; x < part2_expansion_factor; x++) {
                discover_subgrid(next, x, y);
            }
        }

        fmt::print("{}: {} subgrids found\n", i + 1,
                   discovered_subgrid_indexes.size());

        if (previous_subgrid_count < discovered_subgrid_indexes.size()) {
            last_subgrid_count_change = i;
        }
        previous_subgrid_count = discovered_subgrid_indexes.size();
        // print_grid(next);
    }

    fmt::print("Subgrids found: {}\n", discovered_subgrid_indexes.size());
    fmt::print("Last change: {}\n", last_subgrid_count_change);

    std::vector<int> subgrid_first_appearance(
        discovered_subgrid_indexes.size());
    r::fill(subgrid_first_appearance, -1);

    std::vector<std::vector<int>> subgrid_counts_by_step(
        grids_by_step.size(),
        std::vector<int>(discovered_subgrid_indexes.size(), 0));

    for (std::size_t i{0}; i < grids_by_step.size(); i++) {
        fmt::print("{}:\n", i);
        const grid_t& grid{grids_by_step[i]};
        for (int y{0}; y < part2_expansion_factor; y++) {
            for (int x{0}; x < part2_expansion_factor; x++) {
                auto subgrid{grid.subgrid(
                    {{x * start_grid.width(), y * start_grid.height()},
                     {start_grid.width(), start_grid.height()}})};

                auto hash{grid_hash<decltype(subgrid)>{}(subgrid)};
                int index{discovered_subgrid_indexes.at(hash)};
                if (subgrid_first_appearance[index] == -1) {
                    subgrid_first_appearance[index] = static_cast<int>(i);
                }
                subgrid_counts_by_step[i][index]++;
                fmt::print("{:4} ", index);
            }
            fmt::print("\n\n");
        }
        fmt::print("\n");
    }

    fmt::print("First appearances: {}\n\n",
               subgrid_first_appearance | rv::enumerate);

    auto sorted_final_counts_by_step{subgrid_counts_by_step.back() |
                                     rv::enumerate | r::to<std::vector>};
    r::sort(sorted_final_counts_by_step, std::greater{},
            [](auto tuple) { return std::get<1>(tuple); });

    fmt::print("Counts at end: {}\n\n", sorted_final_counts_by_step);

    for (int i{0}; i < static_cast<int>(discovered_subgrids_by_index.size());
         i++) {

        fmt::print("Subgrid {}: ", i);
        for (int j{0}; j < static_cast<int>(subgrid_counts_by_step.size()); j++) {
            if (subgrid_counts_by_step[j][i] > 0) {
                fmt::print("{0:green}{1}{0:reset} ", dh::color{}, subgrid_counts_by_step[j][i]);
            }
            else {
                fmt::print("{0:red}{1}{0:reset} ", dh::color{}, subgrid_counts_by_step[j][i]);
            }
        }
        fmt::print("\n");
    }

    // TODO:

    // For each subgrid index, identify:
    //   The first step in which it appears
    //   The period at which it reappears
    //   The increase in count
    // O(1) Function to compute the count of a given index at a given step

    return {part1, ""};
}

}  // namespace aoc::year2023
