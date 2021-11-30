
add_subdirectory(aoc)

add_subdirectory(2015)
add_subdirectory(2021)

add_library(aoc_solutions aoc_solutions.cpp aoc_solutions.hpp)
target_include_directories(aoc_solutions INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(aoc_solutions PUBLIC project_options fmt::fmt aoc_lib
                                    PRIVATE project_warnings aoc2015 aoc2021)

add_executable(runner main.cpp runner_options.cpp runner_options.hpp)
target_link_libraries(runner PRIVATE project_options project_warnings aoc_solutions fmt::fmt cxxopts::cxxopts tl::expected dh::term)
