add_subdirectory(md5)

add_library(aoc2015 year2015.hpp day01.cpp day02.cpp day03.cpp day04.cpp day05.cpp day06.cpp day06.hpp)
target_include_directories(aoc2015 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(aoc2015 PUBLIC project_options
                              PRIVATE project_warnings aoc_lib md5)

# add_executable(day06vis day06vis.cpp)
# target_link_libraries(day06vis)
