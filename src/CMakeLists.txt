add_library(aoc_fixture aoc_fixture.cpp aoc_fixture.hpp)
target_include_directories(aoc_fixture INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(aoc_fixture PUBLIC project_options 
                                  PRIVATE project_warnings)

add_executable(aoc main.cpp)
target_link_libraries(aoc PRIVATE project_options project_warnings aoc_fixture fmt::fmt)
