add_library(aoc2015 year2015.hpp day01.cpp day02.cpp day03.cpp)
target_include_directories(aoc2015 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(aoc2015 PUBLIC project_options
                              PRIVATE project_warnings aoc_lib)
