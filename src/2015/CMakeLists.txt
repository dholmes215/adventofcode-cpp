add_subdirectory(md5)

add_library(aoc_gate gate.cpp gate.hpp)
target_include_directories(aoc_gate INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(aoc_gate PUBLIC project_options
                               PRIVATE project_warnings aoc_lib ctre::ctre)

add_library(aoc2015 year2015.hpp day01.cpp day02.cpp day03.cpp day04.cpp day05.cpp day06.cpp day06.hpp day07.cpp
    day08.cpp day09.cpp day10.cpp day11.cpp day12.cpp)
target_include_directories(aoc2015 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(aoc2015 PUBLIC project_options
                              PRIVATE project_warnings aoc_lib md5 aoc_gate nlohmann_json::nlohmann_json)

# add_executable(day06vis day06vis.cpp)
# target_link_libraries(day06vis)
