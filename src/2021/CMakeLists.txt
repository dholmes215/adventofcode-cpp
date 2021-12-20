add_library(aoc2021 year2021.hpp day01.cpp day02.cpp day03.cpp day04.cpp day05.cpp day06.cpp day07.cpp 
        day08.cpp day09.cpp day10.cpp day11.cpp day12.cpp day13.cpp day14.cpp day15.cpp day16.cpp day17.cpp
        day18.cpp day19.cpp day20.cpp bits.cpp bits.hpp snailfish.cpp snailfish.hpp)
target_include_directories(aoc2021 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(aoc2021 PUBLIC project_options
        PRIVATE project_warnings aoc_lib ctre::ctre std::mdspan)
