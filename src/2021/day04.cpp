//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_grid.hpp>
#include <aoc_range.hpp>

#include <term.hpp>

#include <fmt/format.h>

#include <string>
#include <vector>

namespace aoc::year2021 {

namespace {

struct bingo_board {
    heap_grid<int, 5, 5> numbers_;
    heap_grid<bool, 5, 5> marked_;
    bool winner_{false};
};

auto all_points() noexcept
{
    return rect<int>{{0, 0}, {5, 5}}.all_points();
}

// void print_board(const bingo_board& board)
// {
//     const dh::color color{};
//     for (int y = 0; y < 5; ++y) {
//         for (int x = 0; x < 5; ++x) {
//             if (board.marked_[{x, y}]) {
//                 fmt::print("{0:yellow}{1:2}{0:reset} ", color, board.numbers_[{x, y}]);
//             } else {
//                 fmt::print("{:2} ", board.numbers_[{x, y}]);
//             }
//         }
//         fmt::print("\n");
//     }
//     fmt::print("\n");
// }

struct bingo_game {
    std::vector<int> called_numbers;
    std::vector<bingo_board> boards;
};

bingo_game parse_input(std::string_view input)
{
    bingo_game output;

    const std::string_view first_line{sv_lines(input).front()};
    output.called_numbers = sv_split_range(first_line, ',') |
                            rv::transform(to_int) | r::to<std::vector>;

    const auto input_rest{input | rv::drop(first_line.size())};
    const auto ints{sv_words(input_rest) | rv::transform(to_int) |
                    r::to<std::vector>};

    const auto board_numbers{ints | rv::chunk(25)};
    output.boards.resize(board_numbers.size());

    // TODO: Make grids regular types so I don't have to do all this
    std::size_t i{0};
    for (auto board_in : board_numbers) {
        r::copy(board_in, output.boards[i].numbers_.data().begin());
        i++;
    }

    return output;
}

struct game_results{
    int first_winner_score{0};
    int last_winner_score{0};
};

game_results play_game(std::vector<bingo_board>& boards,
              const std::vector<int>& called_numbers)
{
    game_results results;
    const int players{static_cast<int>(boards.size())};
    int winners{0};
    for (int called : called_numbers) {
        for (auto& board : boards) {
            for (auto point : all_points()) {
                if (board.numbers_[point] == called) {
                    board.marked_[point] = true;
                }
            }
        }

        for (auto& board : boards) {
            if (board.winner_) {
                continue;
            }

            auto is_true{[](bool b) { return b; }};

            for (auto&& row : board.marked_.rows()) {
                if (r::all_of(row, is_true)) {
                    board.winner_ = true;
                }
            }

            for (auto&& col : board.marked_.cols()) {
                if (r::all_of(col, is_true)) {
                    board.winner_ = true;
                }
            }

            if (board.winner_) {
                // print_board(board);
                int sum{0};
                for (auto point : all_points()) {
                    if (!board.marked_[point]) {
                        sum += board.numbers_[point];
                    }
                }

                int score{sum * called};
                if (winners == 0) {
                    results.first_winner_score = score;
                } else if (winners == players - 1) {
                    results.last_winner_score = score;
                }
                winners++;
            }
        }
    }

    return results;
}

}  // namespace

aoc::solution_result day04(std::string_view input)
{
    bingo_game game{parse_input(input)};
    const game_results result{play_game(game.boards, game.called_numbers)};

    return {result.first_winner_score, result.last_winner_score};
}

}  // namespace aoc::year2021
